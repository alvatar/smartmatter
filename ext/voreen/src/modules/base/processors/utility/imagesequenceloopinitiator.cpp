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

#include "voreen/modules/base/processors/utility/imagesequenceloopinitiator.h"

#include "voreen/modules/base/processors/utility/imagesequenceloopfinalizer.h"
#include "voreen/core/datastructures/imagesequence.h"

namespace voreen {

const std::string ImageSequenceLoopInitiator::loggerCat_("voreen.ImageSequenceLoopInitiator");

ImageSequenceLoopInitiator::ImageSequenceLoopInitiator()
    : RenderProcessor(),
      shader_(0),
      inport_(Port::INPORT, "imagesequence.in"),
      outport_(Port::OUTPORT, "image.out"),
      loopInport_("loop.in", Port::INPORT)
{
    loopInport_.setLoopPort(true);

    addPort(inport_);
    addPort(outport_);
    addPort(loopInport_);
}

std::string ImageSequenceLoopInitiator::getProcessorInfo() const {
    return "Establishes an image processing loop in combination with ImageSequenceLoopFinalizer.             \
           All images of the input sequence are consecutively processed by the loop. Connect this processor's loop inport  \
           with the loop outport of an ImageSequenceLoopFinalizer, in order to define the loop.";
}

Processor* ImageSequenceLoopInitiator::create() const {
    return new ImageSequenceLoopInitiator();
}

bool ImageSequenceLoopInitiator::isReady() const {
    return (inport_.isReady() && outport_.isReady());
}

void ImageSequenceLoopInitiator::process() {

    // get current texture
    tgt::Texture* currentTexture = 0;
    if (loopInport_.getLoopIteration() < (int)inport_.getData()->size()) {
        currentTexture = inport_.getData()->at(loopInport_.getLoopIteration());
    }
    if (!currentTexture) {
        return;
    }

    // adjust outport size to texture dimensions
    outport_.resize(currentTexture->getDimensions().xy());

    // clear outport
    outport_.activateTarget();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    LGL_ERROR;

    // activate shader
    shader_->activate();

    // set common uniforms
    setGlobalShaderParameters(shader_);

    // bind image texture
    glActiveTexture(GL_TEXTURE0);
    currentTexture->bind();

    // pass texture parameters to the shader
    shader_->setUniform("colorTex_", 0);
    shader_->setIgnoreUniformLocationError(true);
    shader_->setUniform("texParams_.dimensions_", tgt::vec2(currentTexture->getDimensions().xy()));
    shader_->setUniform("texParams_.dimensionsRCP_", tgt::vec2(1.f) / tgt::vec2(currentTexture->getDimensions().xy()));
    shader_->setUniform("texParams_.matrix_", tgt::mat4::identity);
    shader_->setIgnoreUniformLocationError(false);

    // execute the shader
    renderQuad();
    LGL_ERROR;

    // clean up
    shader_->deactivate();
    outport_.deactivateTarget();
    LGL_ERROR;
}

void ImageSequenceLoopInitiator::initialize() throw (VoreenException) {

    RenderProcessor::initialize();

    shader_ = ShdrMgr.loadSeparate("passthrough.vert", "copyimage.frag",
        generateHeader() + "#define NO_DEPTH_TEX\n", false);
    if (!shader_) {
        LERROR("Failed to load shaders!");
        initialized_ = false;
        throw VoreenException(getClassName() + ": Failed to load shader!");
    }
}

void ImageSequenceLoopInitiator::deinitialize() throw (VoreenException) {
    ShdrMgr.dispose(shader_);

    RenderProcessor::deinitialize();
}

void ImageSequenceLoopInitiator::invalidate(int inv) {
    RenderProcessor::invalidate(inv);

    if (!inport_.hasData())
        loopInport_.setNumLoopIterations(0);
    else if ((int)inport_.getData()->size() != loopInport_.getNumLoopIterations())
        loopInport_.setNumLoopIterations(inport_.getData()->size());
}

} // voreen namespace
