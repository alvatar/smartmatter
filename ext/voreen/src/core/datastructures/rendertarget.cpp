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

#include "voreen/core/datastructures/rendertarget.h"

#include "tgt/logmanager.h"
#include "tgt/gpucapabilities.h"

using tgt::Texture;

namespace voreen {

const std::string RenderTarget::loggerCat_ = "voreen.RenderTarget";

RenderTarget::RenderTarget()
    : fbo_(0),
    colorTex_(0),
    depthTex_(0),
    numUpdates_(0)
{
}

RenderTarget::~RenderTarget() {
    if (fbo_ || colorTex_ || depthTex_) {
        LERROR("~RenderTarget(): not deinitialized before destruction");
    }
}

void RenderTarget::initialize(GLint internalColorFormat, GLint internalDepthFormat) {

    if (fbo_)
        return;

    if (!GpuCaps.isNpotSupported() && !GpuCaps.areTextureRectanglesSupported()) {
        LWARNING("Neither non-power-of-two textures nor texture rectangles seem to be supported!");
    }

    bool textureRectangles = !GpuCaps.isNpotSupported() && GpuCaps.areTextureRectanglesSupported();

    tgt::ivec3 size(128, 128, 1);

    switch(internalColorFormat) {
        case GL_RGB:
            colorTex_ = new Texture(0, size, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, Texture::LINEAR, textureRectangles);
            break;
        case GL_RGBA:
            colorTex_ = new Texture(0, size, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, Texture::LINEAR, textureRectangles);
            break;
        case GL_RGBA16:
            colorTex_ = new Texture(0, size, GL_RGBA, GL_RGBA16, GL_UNSIGNED_SHORT, Texture::LINEAR, textureRectangles);
            break;
        case GL_RGB16F_ARB:
            colorTex_ = new Texture(0, size, GL_RGB, GL_RGB16F_ARB, GL_FLOAT, Texture::LINEAR, textureRectangles);
            break;
        case GL_RGBA16F_ARB:
            colorTex_ = new Texture(0, size, GL_RGBA, GL_RGBA16F_ARB, GL_FLOAT, Texture::LINEAR, textureRectangles);
            break;
        case GL_RGBA32F_ARB:
            colorTex_ = new Texture(0, size, GL_RGBA, GL_RGBA32F_ARB, GL_FLOAT, Texture::LINEAR, textureRectangles);
            break;
        default:
            LERROR("Unknown internal format!");
    }
    colorTex_->uploadTexture();
    colorTex_->setWrapping(tgt::Texture::CLAMP_TO_EDGE);

    switch(internalDepthFormat) {
        case GL_DEPTH_COMPONENT16:
            depthTex_ = new Texture(0, size, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_FLOAT, Texture::LINEAR, textureRectangles);
            break;
        case GL_DEPTH_COMPONENT24:
            depthTex_ = new Texture(0, size, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT, Texture::LINEAR, textureRectangles);
            break;
        case GL_DEPTH_COMPONENT32:
            depthTex_ = new Texture(0, size, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32, GL_FLOAT, Texture::LINEAR, textureRectangles);
            break;
#ifdef GL_DEPTH_COMPONENT32F
        case GL_DEPTH_COMPONENT32F:
            depthTex_ = new Texture(0, size, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F, GL_FLOAT, Texture::LINEAR, textureRectangles);
            break;
#endif
        default:
            depthTex_ = new Texture(0, size, GL_DEPTH_COMPONENT, internalDepthFormat, GL_FLOAT, Texture::LINEAR, textureRectangles);
            LERROR("Unknown internal depth format!");
    }
    depthTex_->uploadTexture();
    depthTex_->setWrapping(tgt::Texture::CLAMP_TO_EDGE);

    fbo_ = new tgt::FramebufferObject();
    if (!fbo_) {
        LERROR("Failed to initialize framebuffer object!");
        return;
    }
    fbo_->activate();

    fbo_->attachTexture(colorTex_);
    fbo_->isComplete();

    fbo_->attachTexture(depthTex_, GL_DEPTH_ATTACHMENT_EXT);
    fbo_->isComplete();

    fbo_->deactivate();
}

void RenderTarget::deinitialize() {
    delete fbo_;
    fbo_ = 0;

    delete colorTex_;
    colorTex_ = 0;

    delete depthTex_;
    depthTex_ = 0;
}

void RenderTarget::activateTarget(const std::string& debugLabel) {
    if (!fbo_) {
        LERROR("activateTarget(): not initialized");
        return;
    }
    fbo_->activate();
    glViewport(0, 0, colorTex_->getWidth(), colorTex_->getHeight());
    debugLabel_ = debugLabel;
}

void RenderTarget::deactivateTarget() {
    if (fbo_)
        fbo_->deactivate();
}

bool RenderTarget::isActive() const {
    return (fbo_ && fbo_->isActive());
}

void RenderTarget::resize(tgt::ivec2 newsize) {
    if (!fbo_) {
        //LERROR("resize(): not initialized");
        return;
    }

    // no change
    if (newsize == colorTex_->getDimensions().xy())
        return;

    // recreate fbo textures
    glActiveTexture(GL_TEXTURE0);
    colorTex_->destroy();
    colorTex_->setDimensions(tgt::ivec3(newsize.x, newsize.y, 1));
    colorTex_->uploadTexture();

    depthTex_->destroy();
    depthTex_->setDimensions(tgt::ivec3(newsize.x, newsize.y, 1));
    depthTex_->uploadTexture();

    //clearAllTargets();
}

void RenderTarget::bindColorTexture() {
    tgtAssert(colorTex_, "No color texture available!");
    if (colorTex_)
        colorTex_->bind();
}

void RenderTarget::bindColorTexture(GLint texUnit) {
    glActiveTexture(texUnit);
    bindColorTexture();
}

void RenderTarget::bindDepthTexture() {
    tgtAssert(depthTex_, "No depth texture available!");
    if(depthTex_)
        depthTex_->bind();
}

void RenderTarget::bindDepthTexture(GLint texUnit) {
    glActiveTexture(texUnit);
    bindDepthTexture();
}

tgt::ivec2 RenderTarget::getSize() const {
    if (colorTex_)
        return colorTex_->getDimensions().xy();
    else if (depthTex_)
        return depthTex_->getDimensions().xy();
    else
        return tgt::ivec2(0,0);
}

tgt::vec4 RenderTarget::getColorAtPos(tgt::ivec2 pos) {
    activateTarget();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    tgt::vec4 pixels;
    glReadPixels(pos.x, pos.y, 1, 1, GL_RGBA, GL_FLOAT, &pixels);
    deactivateTarget();
    return pixels;
}



}   // namespace
