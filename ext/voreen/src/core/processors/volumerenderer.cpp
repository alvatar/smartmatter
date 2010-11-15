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

#include "tgt/gpucapabilities.h"
#include "tgt/textureunit.h"
#include "voreen/core/processors/volumerenderer.h"

using tgt::vec3;
using tgt::vec4;
using tgt::Color;
using tgt::TextureUnit;

namespace voreen {

const std::string VolumeRenderer::loggerCat_("voreen.VolumeRenderer");

VolumeRenderer::VolumeRenderer()
    : RenderProcessor()
    , lightPosition_("lightPosition", "Light Source Position", tgt::vec4(2.3f, 1.5f, 1.5f, 1.f),
                     tgt::vec4(-10), tgt::vec4(10))
    , lightAmbient_("lightAmbient", "Ambient Light", tgt::vec4(0.4f, 0.4f, 0.4f, 1.f))
    , lightDiffuse_("lightDiffuse", "Diffuse Light", tgt::vec4(0.8f, 0.8f, 0.8f, 1.f))
    , lightSpecular_("lightSpecular", "Specular Light", tgt::vec4(0.6f, 0.6f, 0.6f, 1.f))
    , lightAttenuation_("lightAttenuation", "Attenuation", tgt::vec3(1.f, 0.f, 0.f))
    , applyLightAttenuation_("applyLightAttenuation", "Apply Attenuation", false, Processor::INVALID_PROGRAM)
    , materialAmbient_("materialAmbient", "Ambient material color", tgt::vec4(1.f, 1.f, 1.f, 1.f))
    , materialDiffuse_("materialDiffuse", "Diffuse material color", tgt::vec4(1.f, 1.f, 1.f, 1.f))
    , materialSpecular_("materialSpecular", "Specular material color", tgt::vec4(1.f, 1.f, 1.f, 1.f))
    , materialShininess_("materialShininess", "Shininess", 60.f, 0.1f, 128.f)
{
    lightPosition_.setViews(Property::View(Property::LIGHT_POSITION | Property::DEFAULT));
    lightAmbient_.setViews(Property::COLOR);
    lightDiffuse_.setViews(Property::COLOR);
    lightSpecular_.setViews(Property::COLOR);
    materialAmbient_.setViews(Property::COLOR);
    materialDiffuse_.setViews(Property::COLOR);
}

std::string VolumeRenderer::generateHeader(VolumeHandle* /*volumehandle*/) {
    std::string header = RenderProcessor::generateHeader();

    if (isInitialized() && GpuCaps.isNpotSupported())
        header += "#define VRN_TEXTURE_3D\n";
    else
        header += "#define VRN_TEXTURE_3D_SCALED\n";

    return header;
}

void VolumeRenderer::setGlobalShaderParameters(tgt::Shader* shader, const tgt::Camera* camera) {
    RenderProcessor::setGlobalShaderParameters(shader, camera);

    shader->setIgnoreUniformLocationError(true);

    // light source position in world coordinates
    shader->setUniform("lightSource_.position_", lightPosition_.get().xyz());
    shader->setUniform("lightSource_.attenuation_", lightAttenuation_.get());
    shader->setUniform("lightSource_.ambientColor_", lightAmbient_.get().xyz());
    shader->setUniform("lightSource_.diffuseColor_", lightDiffuse_.get().xyz());
    shader->setUniform("lightSource_.specularColor_", lightSpecular_.get().xyz());

    shader->setUniform("shininess_", materialShininess_.get());

    shader->setIgnoreUniformLocationError(false);
}

void VolumeRenderer::bindVolumes(tgt::Shader* shader, const std::vector<VolumeStruct>& volumes,
                                 const tgt::Camera* camera, const tgt::vec4& lightPosition) {
    bool texCoordScaling = !GpuCaps.isNpotSupported();
    shader->setIgnoreUniformLocationError(true);

    for (size_t i=0; i < volumes.size(); ++i) {
        // some shortcuts
        const VolumeStruct& volumeStruct = volumes[i];
        const VolumeGL* volumeGL = volumeStruct.volume_;
        if (!volumeGL || !volumeGL->getTexture()) {
            LWARNING("No volume texture while binding volumes");
            continue;
        }
        const Volume* volume = volumeGL->getVolume();
        const VolumeTexture* volumeTex = volumeGL->getTexture();

        // bind volume texture and pass sampler to the shader
        GLint loc = shader->getUniformLocation(volumeStruct.samplerIdentifier_);

        TextureUnit* texUnit = volumes[i].texUnit_;
        if (!texUnit) {
            LERROR("No texture unit while binding volumes");
            continue;
        }

        if (loc != -1) {
            texUnit->activate();

            volumeTex->bind();

            // Returns the residence status of the target texture. If the value returned in params is
            // GL_TRUE, the texture is resident in texture memory
            GLint resident;
            glGetTexParameteriv(GL_TEXTURE_3D, GL_TEXTURE_RESIDENT, &resident);

            if (resident != GL_TRUE)
                LWARNING("texture not resident: " /*<< volume->meta().getFileName()*/);

            shader->setUniform(loc, texUnit->getUnitNumber());

            LGL_ERROR;

            // texture filtering
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, volumeStruct.filterMode_);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, volumeStruct.filterMode_);
            LGL_ERROR;

            // texture wrapping
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, volumeStruct.wrapMode_);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, volumeStruct.wrapMode_);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, volumeStruct.wrapMode_);
            glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, volumeStruct.borderColor_.elem);
            LGL_ERROR;
        }

        // set volume meta-data
        std::string paramsIdent = volumeStruct.volumeParametersIdentifier_;
        // volume size, i.e. dimensions of the proxy geometry in world coordinates
        shader->setUniform(paramsIdent + ".datasetDimensions_", tgt::vec3(volume->getDimensions()));
        shader->setUniform(paramsIdent + ".datasetDimensionsRCP_", vec3(1.f) / tgt::vec3(volume->getDimensions()));

        // volume spacing, i.e. voxel size
        shader->setUniform(paramsIdent + ".datasetSpacing_", volume->getSpacing());
        shader->setUniform(paramsIdent + ".datasetSpacingRCP_", vec3(1.f) / volume->getSpacing());

        // volume's size in its object coordinates
        shader->setUniform(paramsIdent + ".volumeCubeSize_", volume->getCubeSize());
        shader->setUniform(paramsIdent + ".volumeCubeSizeRCP_", vec3(1.f) / volume->getCubeSize());

        // volume's transformation matrix
        tgt::mat4 tm = volumeStruct.applyDatasetTrafoMatrix_ ? volume->getTransformation() : tgt::mat4::identity;
        shader->setUniform(paramsIdent + ".volumeTransformation_", tm);

        tgt::mat4 invTm;
        if (!tm.invert(invTm))
            LWARNING("Failed to invert volume transformation matrix!");
        shader->setUniform(paramsIdent + ".volumeTransformationINV_", invTm);

        // camera position in volume object coords
        if (camera)
            shader->setUniform(paramsIdent + ".cameraPositionOBJ_", invTm*camera->getPosition());

        // light position in volume object coords
        shader->setUniform(paramsIdent + ".lightPositionOBJ_", (invTm*lightPosition).xyz());

        LGL_ERROR;

        // scaling of texture coords, if a resize of a npot texture to pot dimensions was necessary
        if (texCoordScaling) {
            // we are only interested in the scaling part of the texture matrix
            vec3 texScaleVector = volumeTex->getMatrix().getScalingPart();
            shader->setUniform(paramsIdent + ".texCoordScaleFactor_", texScaleVector);
            shader->setUniform(paramsIdent + ".texCoordScaleFactorRCP_", vec3(1.f) / texScaleVector);
            LGL_ERROR;
        }

        // bit depth of the volume
        loc = shader->setUniform(paramsIdent + ".bitDepth_", volume->getBitsStored());

        // is the volume a 12 bit volume => fetched texel values have to be normalized in the shader
        if (volume->getBitsStored() == 12)
            shader->setUniform(paramsIdent + ".bitDepthScale_", 16.0f);
        else
            shader->setUniform(paramsIdent + ".bitDepthScale_", 1.0f);

        LGL_ERROR;
    }

    shader->setIgnoreUniformLocationError(false);
    LGL_ERROR;
}

VolumeRenderer::VolumeStruct::VolumeStruct()
    : volume_(0)
{}

VolumeRenderer::VolumeStruct::VolumeStruct(const VolumeGL* volume, tgt::TextureUnit* texUnit,
                                           const std::string& samplerIdentifier,
                                           const std::string& volumeParametersIdentifier,
                                           bool applyDatasetTrafoMatrix, GLenum wrapMode,
                                           tgt::vec4 borderColor, GLint filterMode)
    : volume_(volume),
      texUnit_(texUnit),
      wrapMode_(wrapMode),
      borderColor_(borderColor),
      filterMode_(filterMode),
      samplerIdentifier_(samplerIdentifier),
      volumeParametersIdentifier_(volumeParametersIdentifier),
      applyDatasetTrafoMatrix_(applyDatasetTrafoMatrix)
{}

} // namespace voreen
