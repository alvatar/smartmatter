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

#include "voreen/modules/base/processors/render/rgbraycaster.h"

#include "voreen/core/properties/cameraproperty.h"

#include "tgt/textureunit.h"

using tgt::vec3;
using tgt::TextureUnit;

namespace voreen {

RGBRaycaster::RGBRaycaster()
    : VolumeRaycaster()
    , volumePort_(Port::INPORT, "volumehandle.volumehandle")
    , entryPort_(Port::INPORT, "image.entrypoints")
    , exitPort_(Port::INPORT, "image.exitpoints")
    , outport_(Port::OUTPORT, "image.output", true)
    , raycastPrg_(0)
    , transferFunc_("transferFunction", "Transfer function", Processor::INVALID_RESULT,
        TransFuncProperty::Editors(TransFuncProperty::INTENSITY))
    , applyColorModulation_("applyColorModulation", "Apply color modulation", false)
    , camera_("camera", "Camera", tgt::Camera(vec3(0.f, 0.f, 3.5f), vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f)))
{
    addPort(volumePort_);
    addPort(entryPort_);
    addPort(exitPort_);
    addPort(outport_);

    addProperty(transferFunc_);
    addProperty(applyColorModulation_);
    addProperty(camera_);
    camera_.setVisible(false);
}

std::string RGBRaycaster::getProcessorInfo() const {
    return "Performs raycasting of an RGB volume data set by applying a hue-based transfer function.";
}

Processor* RGBRaycaster::create() const {
    return new RGBRaycaster();
}

void RGBRaycaster::initialize() throw (VoreenException) {

    VolumeRaycaster::initialize();

    loadShader();
    if (!raycastPrg_)
        throw VoreenException(getClassName() + ": Failed to load shaders!");
}

void RGBRaycaster::deinitialize() throw (VoreenException) {
    ShdrMgr.dispose(raycastPrg_);
    raycastPrg_ = 0;
    LGL_ERROR;

    VolumeRaycaster::deinitialize();
}

void RGBRaycaster::loadShader() {
    raycastPrg_ = ShdrMgr.loadSeparate("passthrough.vert", "rc_rgb.frag",
                                       generateHeader(), false);
}

void RGBRaycaster::compile(VolumeHandle* volumeHandle) {
    if (!raycastPrg_)
        return;

    raycastPrg_->setHeaders(generateHeader(volumeHandle));
    raycastPrg_->rebuild();
}

void RGBRaycaster::process() {

    if (!volumePort_.isReady())
        return;

    if (!outport_.isReady())
        return;

    outport_.activateTarget();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // compile program
    if (getInvalidationLevel() >= Processor::INVALID_PROGRAM)
        compile(volumePort_.getData());
    LGL_ERROR;

    TextureUnit entryUnit, entryDepthUnit, exitUnit, exitDepthUnit;
    // bind entry params
    entryPort_.bindTextures(entryUnit.getEnum(), entryDepthUnit.getEnum());

    // bind exit params
    exitPort_.bindTextures(exitUnit.getEnum(), exitDepthUnit.getEnum());

    // vector containing the volumes to bind; is passed to bindVolumes()
    std::vector<VolumeStruct> volumeTextures;

    // add main volume
    TextureUnit volUnit;
    volumeTextures.push_back(VolumeStruct(
        volumePort_.getData()->getVolumeGL(),
        &volUnit,
        "volume_",
        "volumeParameters_",
        true)
    );

    updateBrickingParameters(volumePort_.getData());
    addBrickedVolumeModalities(volumePort_.getData(), volumeTextures);

    // bind transfer function
    TextureUnit transferUnit;
    transferUnit.activate();
    if (transferFunc_.get())
        transferFunc_.get()->bind();

    // initialize shader
    raycastPrg_->activate();

    // set common uniforms used by all shaders
    tgt::Camera cam = camera_.get();
    setGlobalShaderParameters(raycastPrg_, &cam);
    // bind the volumes and pass the necessary information to the shader
    bindVolumes(raycastPrg_, volumeTextures, &cam, lightPosition_.get());

    // pass the remaining uniforms to the shader
    raycastPrg_->setUniform("entryPoints_", entryUnit.getUnitNumber());
    raycastPrg_->setUniform("entryPointsDepth_", entryDepthUnit.getUnitNumber());
    entryPort_.setTextureParameters(raycastPrg_, "entryParameters_");
    raycastPrg_->setUniform("exitPoints_", exitUnit.getUnitNumber());
    raycastPrg_->setUniform("exitPointsDepth_", exitDepthUnit.getUnitNumber());
    exitPort_.setTextureParameters(raycastPrg_, "exitParameters_");
    raycastPrg_->setUniform("transferFunc_", transferUnit.getUnitNumber());
    raycastPrg_->setUniform("applyColorModulation_", applyColorModulation_.get());

    setBrickedVolumeUniforms(raycastPrg_, volumePort_.getData());

    renderQuad();

    raycastPrg_->deactivate();
    TextureUnit::setZeroUnit();

    outport_.deactivateTarget();
    LGL_ERROR;
}

std::string RGBRaycaster::generateHeader(VolumeHandle* volumeHandle) {
    std::string header = VolumeRaycaster::generateHeader(volumeHandle);

    header += transferFunc_.get()->getShaderDefines();

    return header;
}


} // namespace voreen
