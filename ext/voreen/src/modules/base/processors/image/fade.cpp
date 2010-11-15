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

#include "voreen/modules/base/processors/image/fade.h"

#include "tgt/textureunit.h"

using tgt::TextureUnit;

namespace voreen {

Fade::Fade()
    : ImageProcessorDepth("image/fade"),
      fading_("fading", "Fade factor", 0.0f),
      color_("color", "Color", tgt::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
      inport_(Port::INPORT, "image.inport"),
      outport_(Port::OUTPORT, "image.outport")
{
    color_.setViews(Property::COLOR);
    addProperty(fading_);
    addProperty(color_);

    addPort(inport_);
    addPort(outport_);
}

std::string Fade::getProcessorInfo() const {
    return "Performs a fading of the input image towards the specified color.";
}

void Fade::process() {
    outport_.activateTarget();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    TextureUnit colorUnit, depthUnit;
    inport_.bindTextures(colorUnit.getEnum(), depthUnit.getEnum());

    // intialize shader
    program_->activate();
    setGlobalShaderParameters(program_);
    program_->setUniform("colorTex_", colorUnit.getUnitNumber());
    program_->setUniform("depthTex_", depthUnit.getUnitNumber());
    inport_.setTextureParameters(program_, "texParams_");
    program_->setUniform("fading_", 1-fading_.get());
    program_->setUniform("fadeColor_",color_.get());

    renderQuad();

    program_->deactivate();
    outport_.deactivateTarget();
    LGL_ERROR;
}

} // voreen namespace
