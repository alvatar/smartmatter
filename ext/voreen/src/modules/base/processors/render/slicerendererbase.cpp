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

#include <math.h>
#include <limits>
#include <algorithm>
#include <sstream>

#include "tgt/tgt_gl.h"
#include "tgt/gpucapabilities.h"

#include "tgt/plane.h"
#include "tgt/glmath.h"
#include "tgt/textureunit.h"

#include "voreen/modules/base/processors/render/slicerendererbase.h"
#include "voreen/core/datastructures/volume/modality.h"

using tgt::vec2;
using tgt::vec3;
using tgt::vec4;
using tgt::ivec2;
using tgt::ivec3;
using tgt::TextureUnit;

namespace voreen {

SliceRendererBase::SliceRendererBase()
    : VolumeRenderer(),
    transferFunc_("transferFunction", "Transfer Function"),
    texFilterMode_("textureFilterMode", "Texture Filtering"),
    texClampMode_("textureClampMode_", "Texture Clamp"),
    texBorderIntensity_("textureBorderIntensity", "Texture Border Intensity", 0.f),
    sliceShader_(0),
    outport_(Port::OUTPORT, "image.outport"),
    inport_(Port::INPORT, "volumehandle.volumehandle")
{
    addPort(outport_);
    addPort(inport_);

    addProperty(transferFunc_);

    // volume texture filtering
    texFilterMode_.addOption("nearest", "Nearest",  GL_NEAREST);
    texFilterMode_.addOption("linear",  "Linear",   GL_LINEAR);
    texFilterMode_.selectByKey("linear");
    addProperty(texFilterMode_);

    // volume texture clamping
    texClampMode_.addOption("clamp",           "Clamp",             GL_CLAMP);
    texClampMode_.addOption("clamp-to-edge",   "Clamp to Edge",     GL_CLAMP_TO_EDGE);
    texClampMode_.addOption("clamp-to-border", "Clamp to Border",   GL_CLAMP_TO_BORDER);
    texClampMode_.selectByKey("clamp-to-edge");
    addProperty(texClampMode_);
    addProperty(texBorderIntensity_);

    // assign texture access properties to property group
    texFilterMode_.setGroupID("textureAccess");
    texClampMode_.setGroupID("textureAccess");
    texBorderIntensity_.setGroupID("textureAccess");
    setPropertyGroupGuiName("textureAccess", "Volume Texture Access");

    texClampMode_.onChange(CallMemberAction<SliceRendererBase>(this,
        &SliceRendererBase::adjustPropertyVisibilities));
}

void SliceRendererBase::initialize() throw (VoreenException) {
    VolumeRenderer::initialize();

    adjustPropertyVisibilities();
}

void SliceRendererBase::deinitialize() throw (VoreenException) {
    ShdrMgr.dispose(sliceShader_);
    sliceShader_ = 0;

    VolumeRenderer::deinitialize();
}

// protected methods
//

bool SliceRendererBase::ready() const {

    if (!inport_.isReady())
        return false;

    // we need at least one texture
    VolumeGL* volumeGL = inport_.getData()->getVolumeGL();
    if (!volumeGL || volumeGL->getNumTextures() == 0)
        return false;

    bool ready = false;

    // transferfunction and transferfuncshader are needed, if shaders are supported
    if (GpuCaps.areShadersSupported()) {
        ready = (sliceShader_ != 0);
    }
    // without shaders, the texture's format has to be RGBA or luminance-alpha
    else {
        ready = ((volumeGL->getTexture()->getFormat() == GL_LUMINANCE_ALPHA)
                 || (volumeGL->getTexture()->getFormat() == GL_RGBA));
    }

    if (!ready)
        LWARNING("SliceRendererBase not ready to render!");

    return ready;
}


bool SliceRendererBase::setupShader(VolumeGL* volume, TextureUnit* volUnit, TextureUnit* transferUnit,
    const tgt::Camera* camera, const tgt::vec4& lightPosition) {

    // if we don't have hardware support -> return
    if (!GpuCaps.areShadersSupported())
        return false;

    if (!sliceShader_) {
        sliceShader_ = ShdrMgr.load("sl_base", buildShaderHeader(), false);
        LGL_ERROR;
    }

    // activate the shader if everything went fine and set the needed uniforms
    if (sliceShader_) {
        sliceShader_->activate();

        if (transferUnit)
            sliceShader_->setUniform("transferFunc_", transferUnit->getUnitNumber());

        LGL_ERROR;

        // bind volume
        if (volume) {
            std::vector<VolumeStruct> volumeTextures;
            volumeTextures.push_back(VolumeStruct(
                volume,
                volUnit,
                "volume_",
                "volumeParameters_",
                true,
                texClampMode_.getValue(),
                tgt::vec4(texBorderIntensity_.get()),
                texFilterMode_.getValue())
                );
            bindVolumes(sliceShader_, volumeTextures, camera, lightPosition);

            LGL_ERROR;
        }
    }

    return (sliceShader_ != 0);
}

std::string SliceRendererBase::buildShaderHeader() {
    std::string header = VolumeRenderer::generateHeader();

    header += transferFunc_.get()->getShaderDefines();

    return header;
}

bool SliceRendererBase::rebuildShader() {
    // do nothing if there is no shader at the moment
    if (!sliceShader_)
        return false;

    sliceShader_->setHeaders(buildShaderHeader());
    return sliceShader_->rebuild();
}

void SliceRendererBase::deactivateShader() {
    if (sliceShader_ && sliceShader_->isActivated())
        sliceShader_->deactivate();
}

void SliceRendererBase::adjustPropertyVisibilities() {
    texBorderIntensity_.setVisible(!texClampMode_.isSelected("clamp-to-edge"));
}


}   // namespace
