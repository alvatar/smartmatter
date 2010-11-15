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

#include "voreen/modules/base/processors/render/singlevolumeraycaster.h"

#include "tgt/textureunit.h"

#include <sstream>

using tgt::vec3;
using tgt::TextureUnit;

namespace voreen {

const std::string SingleVolumeRaycaster::loggerCat_("voreen.SingleVolumeRaycaster");

SingleVolumeRaycaster::SingleVolumeRaycaster()
    : VolumeRaycaster()
    , volumeInport_(Port::INPORT, "volumehandle.volumehandle", false, Processor::INVALID_PROGRAM)
    , entryPort_(Port::INPORT, "image.entrypoints")
    , exitPort_(Port::INPORT, "image.exitpoints")
    , outport_(Port::OUTPORT, "image.output", true, Processor::INVALID_PROGRAM)
    , outport1_(Port::OUTPORT, "image.output1", true, Processor::INVALID_PROGRAM)
    , outport2_(Port::OUTPORT, "image.output2", true, Processor::INVALID_PROGRAM)
    , raycastPrg_(0)
    , transferFunc_("transferFunction", "Transfer Function")
    , camera_("camera", "Camera", tgt::Camera(vec3(0.f, 0.f, 3.5f), vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f)))
    , compositingMode1_("compositing1", "Compositing (OP2)", Processor::INVALID_PROGRAM)
    , compositingMode2_("compositing2", "Compositing (OP3)", Processor::INVALID_PROGRAM)
    , texFilterMode_("textureFilterMode_", "Texture Filtering")
    , texClampMode_("textureClampMode_", "Texture Clamp")
    , texBorderIntensity_("textureBorderIntensity", "Texture Border Intensity", 0.f)
{
    // ports
    addPort(volumeInport_);
    addPort(entryPort_);
    addPort(exitPort_);
    addPort(outport_);
    addPort(outport1_);
    addPort(outport2_);

    // shading / classification props
    addProperty(transferFunc_);
    addProperty(camera_);
    addProperty(gradientMode_);
    addProperty(classificationMode_);
    addProperty(shadeMode_);

    // compositing modes
    addProperty(compositingMode_);
    compositingMode1_.addOption("dvr", "DVR");
    compositingMode1_.addOption("mip", "MIP");
    compositingMode1_.addOption("iso", "ISO");
    compositingMode1_.addOption("fhp", "FHP");
    compositingMode1_.addOption("fhn", "FHN");
    addProperty(compositingMode1_);

    compositingMode2_.addOption("dvr", "DVR");
    compositingMode2_.addOption("mip", "MIP");
    compositingMode2_.addOption("iso", "ISO");
    compositingMode2_.addOption("fhp", "FHP");
    compositingMode2_.addOption("fhn", "FHN");
    addProperty(compositingMode2_);

    addProperty(isoValue_);

    // lighting
    addProperty(lightPosition_);
    addProperty(lightAmbient_);
    addProperty(lightDiffuse_);
    addProperty(lightSpecular_);
    addProperty(materialShininess_);
    addProperty(applyLightAttenuation_);
    addProperty(lightAttenuation_);

    // assign lighting properties to property group
    lightPosition_.setGroupID("lighting");
    lightAmbient_.setGroupID("lighting");
    lightDiffuse_.setGroupID("lighting");
    lightSpecular_.setGroupID("lighting");
    materialShininess_.setGroupID("lighting");
    applyLightAttenuation_.setGroupID("lighting");
    lightAttenuation_.setGroupID("lighting");
    setPropertyGroupGuiName("lighting", "Lighting Parameters");

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

    // listen to changes of properties that influence the GUI state (i.e. visibility of other props)
    classificationMode_.onChange(CallMemberAction<SingleVolumeRaycaster>(this, &SingleVolumeRaycaster::adjustPropertyVisibilities));
    shadeMode_.onChange(CallMemberAction<SingleVolumeRaycaster>(this, &SingleVolumeRaycaster::adjustPropertyVisibilities));
    compositingMode_.onChange(CallMemberAction<SingleVolumeRaycaster>(this, &SingleVolumeRaycaster::adjustPropertyVisibilities));
    compositingMode1_.onChange(CallMemberAction<SingleVolumeRaycaster>(this, &SingleVolumeRaycaster::adjustPropertyVisibilities));
    compositingMode2_.onChange(CallMemberAction<SingleVolumeRaycaster>(this, &SingleVolumeRaycaster::adjustPropertyVisibilities));
    applyLightAttenuation_.onChange(CallMemberAction<SingleVolumeRaycaster>(this, &SingleVolumeRaycaster::adjustPropertyVisibilities));
    texClampMode_.onChange(CallMemberAction<SingleVolumeRaycaster>(this, &SingleVolumeRaycaster::adjustPropertyVisibilities));
}

std::string SingleVolumeRaycaster::getProcessorInfo() const {
    return "This is the standard volume renderer in Voreen. It generates up to three output "
           "renderings, where the first one also provides depth values. Several "
           "shading and compositing modes are supported.<br/>"
           "See CubeMeshProxyGeometry, MeshEntryExitPoints.";
}

Processor* SingleVolumeRaycaster::create() const {
    return new SingleVolumeRaycaster();
}

void SingleVolumeRaycaster::initialize() throw (VoreenException) {
    VolumeRaycaster::initialize();

    raycastPrg_ = ShdrMgr.loadSeparate("passthrough.vert", "rc_singlevolume.frag",
        generateHeader(), false);
    if (!raycastPrg_)
        throw VoreenException("Failed to load shaders (passthrough.vert/rc_singlevolume.frag)");

    portGroup_.initialize();
    portGroup_.addPort(outport_);
    portGroup_.addPort(outport1_);
    portGroup_.addPort(outport2_);

    adjustPropertyVisibilities();

    if (transferFunc_.get()) {
        transferFunc_.get()->getTexture();
        transferFunc_.get()->invalidateTexture();
    }
}

void SingleVolumeRaycaster::deinitialize() throw (VoreenException) {
    portGroup_.deinitialize();
    LGL_ERROR;

    ShdrMgr.dispose(raycastPrg_);
    raycastPrg_ = 0;
    LGL_ERROR;

    VolumeRaycaster::deinitialize();
}

void SingleVolumeRaycaster::compile(VolumeHandle* volumeHandle) {
    raycastPrg_->setHeaders(generateHeader(volumeHandle));
    raycastPrg_->rebuild();
}

bool SingleVolumeRaycaster::isReady() const {
    //check if all inports are connected:
    if(!entryPort_.isReady() || !exitPort_.isReady() || !volumeInport_.isReady())
        return false;

    //check if at least one outport is connected:
    if (!outport_.isReady() && !outport1_.isReady() && !outport2_.isReady())
        return false;

    return true;
}

void SingleVolumeRaycaster::beforeProcess() {
    VolumeRaycaster::beforeProcess();

    // compile program if needed
    if (getInvalidationLevel() >= Processor::INVALID_PROGRAM) {
        PROFILING_BLOCK("compile");
        compile(volumeInport_.getData());
    }
    LGL_ERROR;

    transferFunc_.setVolumeHandle(volumeInport_.getData());
}

void SingleVolumeRaycaster::process() {

    // bind transfer function
    tgt::TextureUnit transferUnit;
    transferUnit.activate();
    LGL_ERROR;
    if (transferFunc_.get())
        transferFunc_.get()->bind();

    portGroup_.activateTargets();
    portGroup_.clearTargets();
    LGL_ERROR;

    // bind entry params
    tgt::TextureUnit entryUnit, entryDepthUnit, exitUnit, exitDepthUnit;
    entryPort_.bindTextures(entryUnit, entryDepthUnit);
    LGL_ERROR;

    // bind exit params
    exitPort_.bindTextures(exitUnit, exitDepthUnit);
    LGL_ERROR;

    // vector containing the volumes to bind; is passed to bindVolumes()
    std::vector<VolumeStruct> volumeTextures;

    // add main volume
    TextureUnit volUnit;
    volumeTextures.push_back(VolumeStruct(
        volumeInport_.getData()->getVolumeGL(),
        &volUnit,
        "volume_",
        "volumeParameters_",
        true,
        texClampMode_.getValue(),
        tgt::vec4(texBorderIntensity_.get()),
        texFilterMode_.getValue())
    );

    updateBrickingParameters(volumeInport_.getData());
    TextureUnit brickUnit1, brickUnit2;
    addBrickedVolumeModalities(volumeInport_.getData(), volumeTextures, &brickUnit1, &brickUnit2);

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

    if (compositingMode_.isSelected("iso")  ||
        compositingMode1_.isSelected("iso") ||
        compositingMode2_.isSelected("iso") )
        raycastPrg_->setUniform("isoValue_", isoValue_.get());

    if (classificationMode_.get() == "transfer-function")
        raycastPrg_->setUniform("transferFunc_", transferUnit.getUnitNumber());

    setBrickedVolumeUniforms(raycastPrg_, volumeInport_.getData());
    LGL_ERROR;

    {
        PROFILING_BLOCK("raycasting");
        renderQuad();
    }

    raycastPrg_->deactivate();
    portGroup_.deactivateTargets();

    TextureUnit::setZeroUnit();
    LGL_ERROR;
}

std::string SingleVolumeRaycaster::generateHeader(VolumeHandle* volumeHandle) {
    std::string headerSource = VolumeRaycaster::generateHeader(volumeHandle);

    headerSource += transferFunc_.get()->getShaderDefines();

    // configure compositing mode for port 2
    headerSource += "#define RC_APPLY_COMPOSITING_1(result, color, samplePos, gradient, t, tDepth) ";
    if (compositingMode1_.isSelected("dvr"))
        headerSource += "compositeDVR(result, color, t, tDepth);\n";
    else if (compositingMode1_.isSelected("mip"))
        headerSource += "compositeMIP(result, color, t, tDepth);\n";
    else if (compositingMode1_.isSelected("iso"))
        headerSource += "compositeISO(result, color, t, tDepth, isoValue_);\n";
    else if (compositingMode1_.isSelected("fhp"))
        headerSource += "compositeFHP(samplePos, result, t, tDepth);\n";
    else if (compositingMode1_.isSelected("fhn"))
        headerSource += "compositeFHN(gradient, result, t, tDepth);\n";

    // configure compositing mode for port 3
    headerSource += "#define RC_APPLY_COMPOSITING_2(result, color, samplePos, gradient, t, tDepth) ";
    if (compositingMode2_.isSelected("dvr"))
        headerSource += "compositeDVR(result, color, t, tDepth);\n";
    else if (compositingMode2_.isSelected("mip"))
        headerSource += "compositeMIP(result, color, t, tDepth);\n";
    else if (compositingMode2_.isSelected("iso"))
        headerSource += "compositeISO(result, color, t, tDepth, isoValue_);\n";
    else if (compositingMode2_.isSelected("fhp"))
        headerSource += "compositeFHP(samplePos, result, t, tDepth);\n";
    else if (compositingMode2_.isSelected("fhn"))
        headerSource += "compositeFHN(gradient, result, t, tDepth);\n";

    portGroup_.reattachTargets();
    headerSource += portGroup_.generateHeader(raycastPrg_);
    return headerSource;
}

void SingleVolumeRaycaster::adjustPropertyVisibilities() {
    bool useLighting = !shadeMode_.isSelected("none");
    setPropertyGroupVisible("lighting", useLighting);

    bool useIsovalue = (compositingMode_.isSelected("iso")  ||
                        compositingMode1_.isSelected("iso") ||
                        compositingMode2_.isSelected("iso")   );
    isoValue_.setVisible(useIsovalue);

    lightAttenuation_.setVisible(applyLightAttenuation_.get());

    texBorderIntensity_.setVisible(!texClampMode_.isSelected("clamp-to-edge"));
}

} // namespace
