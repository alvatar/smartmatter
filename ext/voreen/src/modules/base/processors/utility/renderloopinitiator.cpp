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

#include "voreen/modules/base/processors/utility/renderloopinitiator.h"

#include "tgt/textureunit.h"

using tgt::TextureUnit;

namespace voreen {

RenderLoopInitiator::RenderLoopInitiator()
    : RenderProcessor(),
      shader_(0),
      inport_(Port::INPORT, "image.inport"),
      outport_(Port::OUTPORT, "image.outport"),
      loopInport_(Port::INPORT, "loop.inport"),
      numIterations_("numIterations", "Iterations", 3, 1, 100)
{
    loopInport_.setLoopPort(true);
    loopInport_.setNumLoopIterations(numIterations_.get());

    numIterations_.onChange(CallMemberAction<RenderLoopInitiator>(this, &RenderLoopInitiator::updateIterationCount));
    addProperty(numIterations_);

    addPort(inport_);
    addPort(outport_);
    addPort(loopInport_);
}

std::string RenderLoopInitiator::getProcessorInfo() const {
    return "In combination with RenderLoopFinalizer, this processor is used to establish render loops:  \
           The path between RenderLoopInitiator and RenderLoopFinalizer in the rendering network        \
           is executed as often as specified by this processor's \"Iterations\" property. At the end    \
           of each iteration, the temporary rendering result is passed back from the loop finalizer     \
           to the loop initiator where it is used as input for the next iteration.";
}

Processor* RenderLoopInitiator::create() const {
    return new RenderLoopInitiator();
}

void RenderLoopInitiator::initialize() throw (VoreenException) {
    RenderProcessor::initialize();

    shader_ = ShdrMgr.loadSeparate("passthrough.vert", "copyimage.frag",
                                   generateHeader(), false);

    if (!shader_) {
        LERROR("Failed to load shaders!");
        initialized_ = false;
        throw VoreenException(getClassName() + ": Failed to load shaders!");
    }
}

void RenderLoopInitiator::deinitialize() throw (VoreenException) {
    ShdrMgr.dispose(shader_);

    RenderProcessor::deinitialize();
}

bool RenderLoopInitiator::isReady() const {
    return (inport_.isReady() && outport_.isReady());
}

void RenderLoopInitiator::process() {
    outport_.activateTarget();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // activate shader
    shader_->activate();

    TextureUnit colorUnit, depthUnit;
    if (loopInport_.getLoopIteration() == 0) {
        // first iteration => copy initial input image to outport
        inport_.bindTextures(colorUnit.getEnum(), depthUnit.getEnum());
    }
    else {
        // loop iteration => copy loop input image to outport
        loopInport_.bindTextures(colorUnit.getEnum(), depthUnit.getEnum());
    }
    LGL_ERROR;

    // set common uniforms
    setGlobalShaderParameters(shader_);

    // pass texture parameters to the shader
    shader_->setUniform("colorTex_", colorUnit.getUnitNumber());
    shader_->setUniform("depthTex_", depthUnit.getUnitNumber());
    inport_.setTextureParameters(shader_, "texParams_");

    // execute shader
    renderQuad();

    // clean up
    shader_->deactivate();
    outport_.deactivateTarget();
    LGL_ERROR;
}

void RenderLoopInitiator::updateIterationCount() {
    loopInport_.setNumLoopIterations(numIterations_.get());
}

} // voreen namespace
