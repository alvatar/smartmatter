/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2010 The Voreen Team. <http://www.voreen.org>   *
 *                                                                    *
 * This file is part of the Voreen software package. Voreen is free   *
 * software: you can redistribute it and/or modify it under the terms *
 * of the GNU General Public License version 2 as published by the    *
 * Free Software Foundation.                                          *
 *                                                                    *
 * Voreen is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * in the file "LICENSE.txt" along with this program.                 *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 * The authors reserve all rights not expressly granted herein. For   *
 * non-commercial academic use see the license exception specified in *
 * the file "LICENSE-academic.txt". To get information about          *
 * commercial licensing please contact the authors.                   *
 *                                                                    *
 **********************************************************************/

#include "voreen/modules/base/processors/image/background.h"
#include "voreen/core/voreenapplication.h"

#include "tgt/texturemanager.h"
#include "tgt/gpucapabilities.h"
#include "tgt/textureunit.h"

using tgt::TextureUnit;

namespace voreen {

namespace {

GLubyte* blur(GLubyte* image, int size) {
    GLubyte* n = new GLubyte[size * size];

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int pos = x + y * size;
            n[pos] = image[pos] / 5;

            if (x == 0)
                n[pos] += image[pos + (size - 1)] / 5;
            else
                n[pos] += image[pos - 1] / 5;

            if (y == 0)
                n[pos] += image[pos + (size - 1) * size] / 5;
            else
                n[pos] += image[pos - size] / 5;

            if (x == size-1)
                n[pos] += image[y * size] / 5;
            else
                n[pos] += image[pos + 1] / 5;

            if (y == size-1)
                n[pos] += image[x] / 5;
            else
                n[pos] +=image[pos + size] / 5;
        }
    }
    delete[] image;
    return n;
}

GLubyte* resize(GLubyte* image, int size) {
    GLubyte* n = new GLubyte[4 * size * size];

    for (int y = 0; y < size*2; y++)
        for (int x = 0; x < size*2; x++)
            n[x + y*size*2] = image[(x / 2) + (y / 2) * size];

    delete[] image;

    return blur(blur(n, size*2), size*2);
}

GLubyte* tile(GLubyte* image, int size) {
    GLubyte* n = new GLubyte[4 * size * size];

    for (int y = 0; y < size*2; y++)
        for (int x = 0; x < size*2; x++)
            n[x + y*size*2] = image[(x % size) + (y % size) * size];

    delete[] image;

    return n;
}

} // namespace

Background::Background()
    : ImageProcessor("image/background")
    , firstcolor_("color1", "First Color", tgt::vec4(1.0f, 1.0f, 1.0f, 1.0f))
    , secondcolor_("color2", "Second Color", tgt::vec4(0.2f, 0.2f, 0.2f, 1.0f))
    , angle_("angle", "Angle", 0, 0, 359)
    , tex_(0)
    , textureLoaded_(false)
    , filename_("texture", "Texture", "Select texture",
                VoreenApplication::app()->getTexturePath(), "Image Files (*.jpg *.png *.bmp)",
                FileDialogProperty::OPEN_FILE, Processor::INVALID_RESULT)
    , tile_("repeat", "Repeat Background", 1.0f, 0.f, 100.f)
    , modeProp_("backgroundModeAsString", "Type")
    , inport_(Port::INPORT, "image.input")
    , outport_(Port::OUTPORT, "image.output")
    , privatePort_(Port::OUTPORT, "image.tmp", false)
    , textureInvalid_(true)
{
    firstcolor_.setViews(Property::COLOR);
    secondcolor_.setViews(Property::COLOR);
    modeProp_.addOption("none",       "No Background");
    modeProp_.addOption("monochrome", "Monochrome");
    modeProp_.addOption("gradient",   "Gradient");
    modeProp_.addOption("radial",     "Radial");
    modeProp_.addOption("texture",    "Texture");
    modeProp_.addOption("cloud",      "Cloud");
    modeProp_.select("monochrome");
    modeProp_.onChange(CallMemberAction<Background>(this, &Background::onBackgroundModeChanged));
    onBackgroundModeChanged(); // setup initial property visibilty

    addProperty(modeProp_);
    addProperty(firstcolor_);
    addProperty(secondcolor_);
    firstcolor_.onChange(CallMemberAction<Background>(this, &Background::invalidateTexture));
    secondcolor_.onChange(CallMemberAction<Background>(this, &Background::invalidateTexture));

#ifdef VRN_WITH_DEVIL
    addProperty(filename_);
    filename_.onChange(CallMemberAction<Background>(this, &Background::invalidateTexture));
#endif
    addProperty(angle_);
    addProperty(tile_);

    addPort(inport_);
    addPort(outport_);
    addPrivateRenderPort(&privatePort_);
}

std::string Background::getProcessorInfo() const {
    return "Adds a background, either single color, linear or radial gradient, "
           "procedural clouds or an image file.";
}

Processor* Background::create() const {
    return new Background();
}

void Background::initialize() throw (VoreenException) {
    ImageProcessor::initialize();
    loadTexture();
}

void Background::deinitialize() throw (VoreenException) {
    if (tex_) {
        if (textureLoaded_)
            TexMgr.dispose(tex_);
        else
            delete tex_;
    }
    LGL_ERROR;
    tex_ = 0;

    ImageProcessor::deinitialize();
}

bool Background::isReady() const {
    return outport_.isReady();
}

void Background::process() {
    if (!inport_.isReady())
        outport_.activateTarget();
    else
        privatePort_.activateTarget();

    if (getInvalidationLevel() >= Processor::INVALID_PROGRAM)
        compile();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearDepth(1.0);

    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // first the background
    renderBackground();

    if (!inport_.isReady())
        outport_.deactivateTarget();
    else
        privatePort_.deactivateTarget();

    glEnable(GL_DEPTH_TEST);

    // leave if there's nothing to render above
    if (inport_.isReady()) {
        outport_.activateTarget();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // use the shader to to blend the original picture over the background and to keep the
        // depth values

        TextureUnit colorUnit0, depthUnit0, colorUnit1, depthUnit1;
        privatePort_.bindTextures(colorUnit0.getEnum(), depthUnit0.getEnum());
        inport_.bindTextures(colorUnit1.getEnum(), depthUnit1.getEnum());

        // initialize shader
        program_->activate();
        setGlobalShaderParameters(program_);
        program_->setUniform("colorTex0_", colorUnit0.getUnitNumber());
        program_->setUniform("depthTex0_", depthUnit0.getUnitNumber());
        program_->setUniform("colorTex1_", colorUnit1.getUnitNumber());
        program_->setUniform("depthTex1_", depthUnit1.getUnitNumber());
        privatePort_.setTextureParameters(program_, "textureParameters0_");
        inport_.setTextureParameters(program_, "textureParameters1_");

        glDepthFunc(GL_ALWAYS);
        renderQuad();
        glDepthFunc(GL_LESS);
        outport_.deactivateTarget();

        program_->deactivate();
        TextureUnit::setZeroUnit();
    }
    LGL_ERROR;
}

void Background::renderBackground() {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    if (modeProp_.isSelected("monochrome")) {
        glPushAttrib(GL_COLOR_BUFFER_BIT);
        glClearColor(firstcolor_.get().r, firstcolor_.get().g,
                    firstcolor_.get().b, firstcolor_.get().a);
        glClear(GL_COLOR_BUFFER_BIT);
        glPopAttrib();
        LGL_ERROR;
    }
    else if (modeProp_.isSelected("gradient")) {
        // linear gradient
        glPushMatrix();
        glLoadIdentity();
        glRotatef(static_cast<float>(angle_.get()), 0.0f, 0.0f, 1.0f);

        // when you rotate the texture, you need to scale it.
        // otherwise the edges don't cover the complete screen
        // magic number: 0.8284271247461900976033774484194f = sqrt(8)-2;
        glScalef(1.0f + (45 - abs(angle_.get() % 90 - 45)) / 45.0f*0.8284271247461900976033774484194f,
            1.0f + (45 - abs(angle_.get() % 90 - 45)) / 45.0f*0.8284271247461900976033774484194f, 1.0f);

        glBegin(GL_QUADS);
        glColor4fv(&firstcolor_.get().elem[0]);
        glVertex2f(-1.0, -1.0);
        glVertex2f( 1.0, -1.0);
        glColor4fv(&secondcolor_.get().elem[0]);
        glVertex2f( 1.0, 1.0);
        glVertex2f(-1.0, 1.0);
        glEnd();
        glPopMatrix();
        LGL_ERROR;
    }
    else if (modeProp_.isSelected("radial")) {

        TextureUnit::setZeroUnit();

        if (!tex_ || textureInvalid_)
            loadTexture();

        if (tex_) {
            tex_->bind();
            tex_->enable();
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            LGL_ERROR;

            glPushMatrix();
            glScalef(1.44f, 1.44f, 0.0f);

            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f( -1.0f, -1.0f);
            glTexCoord2f(tile_.get(), 0.0f);
            glVertex2f( 1.0f, -1.0f);
            glTexCoord2f(tile_.get(), tile_.get());
            glVertex2f( 1.0f, 1.0f);
            glTexCoord2f(0.0f, tile_.get());
            glVertex2f( -1.0f, 1.0f);
            glEnd();

            glPopMatrix();

            glDisable(GL_TEXTURE_2D);
            LGL_ERROR;
        }
        else {
            LWARNING("No texture");
        }
    }
    else if (modeProp_.isSelected("texture") || modeProp_.isSelected("none")) {

        if (!textureLoaded_ || textureInvalid_)
            loadTexture();

        if (tex_) {
            glActiveTexture(GL_TEXTURE0);
            tex_->bind();
            tex_->enable();
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            LGL_ERROR;

            glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(tile_.get(), 0.0f);
            glVertex2f( 1.0f, -1.0f);
            glTexCoord2f(tile_.get(), tile_.get());
            glVertex2f( 1.0f, 1.0f);
            glTexCoord2f(0.0f, tile_.get());
            glVertex2f(-1.0f, 1.0f);
            glEnd();

            tex_->disable();
            LGL_ERROR;
        }
    }
    else if (modeProp_.isSelected("cloud")) {

        // cloud texture
        if (!tex_ || textureInvalid_)
            loadTexture();

        if (tex_) {
            glActiveTexture(GL_TEXTURE0);
            tex_->bind();
            tex_->enable();
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            LGL_ERROR;

            glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(tile_.get(), 0.0f);
            glVertex2f( 1.0f, -1.0f);
            glTexCoord2f(tile_.get(), tile_.get());
            glVertex2f( 1.0f, 1.0f);
            glTexCoord2f(0.0f, tile_.get());
            glVertex2f(-1.0f, 1.0f);
            glEnd();

            tex_->disable();
            LGL_ERROR;
        }
        else {
            LWARNING("No texture");
        }
    }
    else {
        LWARNING("Unknown background mode");
    }
}

void Background::onBackgroundModeChanged() {

    if (modeProp_.isSelected("none")) {
        firstcolor_.setVisible(false);
        secondcolor_.setVisible(false);
        angle_.setVisible(false);
        filename_.setVisible(false);
        tile_.setVisible(false);
    }
    else if (modeProp_.isSelected("monochrome")) {
        firstcolor_.setVisible(true);
        secondcolor_.setVisible(false);
        angle_.setVisible(false);
        filename_.setVisible(false);
        tile_.setVisible(false);
    }
    else if (modeProp_.isSelected("gradient")) {
        firstcolor_.setVisible(true);
        secondcolor_.setVisible(true);
        angle_.setVisible(true);
        filename_.setVisible(false);
        tile_.setVisible(false);
    }
    else if (modeProp_.isSelected("radial")) {
        tile_.set(1.f);
        firstcolor_.setVisible(true);
        secondcolor_.setVisible(true);
        angle_.setVisible(false);
        filename_.setVisible(false);
        tile_.setVisible(false);
    }
    else if (modeProp_.isSelected("cloud")) {
        firstcolor_.setVisible(false);
        secondcolor_.setVisible(false);
        angle_.setVisible(false);
        filename_.setVisible(false);
        tile_.setVisible(true);
    }
    else if (modeProp_.isSelected("texture")) {
        firstcolor_.setVisible(false);
        secondcolor_.setVisible(false);
        angle_.setVisible(false);
        filename_.setVisible(true);
        tile_.setVisible(true);
    }

    textureInvalid_ = true;
    invalidate();
}

void Background::loadTexture() {
    // only use OpenGL after processor is initialized
    if (!isInitialized())
        return;

    // is a texture already loaded? -> then delete
    if (tex_) {
        if (textureLoaded_) {
            TexMgr.dispose(tex_);
            textureLoaded_ = false;
        }
        else
            delete tex_;

        LGL_ERROR;
        tex_ = 0;
    }

    // create Texture
    if (modeProp_.isSelected("none")) {
        createEmptyTexture();
    }
    else if (modeProp_.isSelected("radial")) {
        createRadialTexture();
    }
    else if (modeProp_.isSelected("texture")) {
        if (!filename_.get().empty()) {
            tex_ = TexMgr.load(filename_.get(), tgt::Texture::LINEAR, false, false, true, true, !GpuCaps.isNpotSupported());
        }
        if (tex_)
            textureLoaded_ = true;
    }
    else if (modeProp_.isSelected("cloud")) {
        createCloudTexture();
    }
    textureInvalid_ = false;

    // adapt texture's data type to input texture, if necessary
    /*tgt::Texture* portTex = inport_.hasData() ? inport_.getData()->getColorTexture() : 0;
    if (tex_ && portTex && (tex_->getDataType() != portTex->getDataType()) ) {
        delete[] tex_->getPixelData();
        tex_->setPixelData(0);
        GLubyte* convBuffer = tex_->downloadTextureToBuffer(tex_->getFormat(), portTex->getDataType());
        LGL_ERROR;
        tex_->setPixelData(convBuffer);
        tex_->setDataType(portTex->getDataType());
        tex_->setInternalFormat(portTex->getInternalFormat());
        tex_->uploadTexture();
        LGL_ERROR;
    }*/
}

void Background::createCloudTexture() {
    int imgx = 256;
    int imgy = 256;
    int size = 32;

    // creates 4 blured and resized noise-datas
    GLubyte* tex1 = new GLubyte[size * size];

    for (int k = 0; k < size*size; k++)
        tex1[k] = rand() % 256;

    tex1 = tile(tile(tile(blur(tex1, size), size), size * 2), size * 4);
    GLubyte* tex2 = new GLubyte[size * size];

    for (int k = 0; k < size*size; k++)
        tex2[k] = rand() % 256;

    tex2 = tile(tile(resize(blur(tex2, size), size), size * 2), size * 4);
    GLubyte* tex3 = new GLubyte[size * size];

    for (int k = 0; k < size*size; k++) {
        tex3[k] = rand() % 256;
    }

    tex3 = tile(resize(resize(blur(tex3, size), size), size * 2), size * 4);
    GLubyte* tex4 = new GLubyte[size * size];

    for (int k = 0; k < size*size; k++)
        tex4[k] = rand() % 256;

    tex4 = resize(resize(resize(blur(tex4, size), size), size * 2), size * 4);

    // creates the final cloud data with the 4 textures from above
    GLubyte* tex5 = new GLubyte[imgx * imgy];

    for (int y = 0; y < imgy; y++) {
        for (int x = 0; x < imgx; x++) {
            int pos = x + imgx * y;
            // mix textures and don't use all values, so that there are free areas in the sky
            float c = (tex4[pos] + tex3[pos] / 2 + tex2[pos] / 4 + tex1[pos] / 8) / 2 - 110.0f;
            if (c < 0)
                c = 0;
            // exp-function to calc the final value for a nicer look
            tex5[pos] = (GLubyte)(255 - (pow(0.95f, c) * 255));
        }
    }

    tex5 = blur(tex5, imgx);

    tex_ = new tgt::Texture(tgt::ivec3(imgx, imgy, 1));

    for (int y = 0; y < imgy; y++) {
        for (int x = 0; x < imgx; x++) {
            tgt::vec4 cloudCol = tgt::vec4(1.f);
            tgt::vec4 backgroundCol = tgt::vec4(0.5f, 0.5f, 1.f, 1.f);
            tgt::vec4 mixColor = tgt::mix(backgroundCol, cloudCol, tgt::clamp(tex5[x + imgx * y] / 255.f, 0.f, 1.f));
            tex_->texel<tgt::col4>(x,y) = tgt::col4(tgt::iround(mixColor*255.f));
        }
    }

    delete[] tex1;
    delete[] tex2;
    delete[] tex3;
    delete[] tex4;
    delete[] tex5;

    tex_->bind();
    tex_->uploadTexture();
    LGL_ERROR;
}

void Background::createRadialTexture() {
    int imgx = 256;
    int imgy = 256;
    tex_ = new tgt::Texture(tgt::ivec3(imgx, imgy, 1));

    // create pixel data
    float r;
    for (int y = 0; y < imgy; y++) {
        for (int x = 0; x < imgx; x++) {
            // calculate radius (Pythagoras)
            r = static_cast<float>((x - imgx / 2) * (x - imgx / 2) + (y - imgy / 2) * (y - imgy / 2));
            r = sqrt(r);
            // norm to half size of smaller value
            r = 2 * r / ((imgx < imgy) ? imgx : imgy);
            // mix both colors according to radius
            r = tgt::clamp(r, 0.f, 1.f);
            tgt::vec4 mixedColor = tgt::mix(firstcolor_.get(), secondcolor_.get(), r);
            tex_->texel<tgt::col4>(x,y) = tgt::col4(tgt::iround(mixedColor * 255.f));;
        }
    }

    tex_->bind();
    tex_->uploadTexture();
    LGL_ERROR;
}

void Background::createEmptyTexture() {
    int imgx = 256;
    int imgy = 256;
    tex_ = new tgt::Texture(tgt::ivec3(imgx, imgy, 1));

    // create pixel data
    for (int y = 0; y < imgy; y++) {
        for (int x = 0; x < imgx; x++) {
            tex_->texel<tgt::col4>(x,y) = tgt::col4(0, 0, 0, 0);
        }
    }

    tex_->bind();
    tex_->uploadTexture();
    LGL_ERROR;
}

void Background::invalidateTexture() {
    textureInvalid_ = true;
}


} // namespace
