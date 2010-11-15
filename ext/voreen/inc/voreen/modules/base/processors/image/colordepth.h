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

#ifndef VRN_COLORDEPTHPP_H
#define VRN_COLORDEPTHPP_H

#include "voreen/core/processors/imageprocessordepth.h"
#include "voreen/core/voreenapplication.h"

#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/floatproperty.h"

namespace voreen {

/**
 * Performs a color filtering which encodes depth information.
 *
 */
class ColorDepth : public ImageProcessorDepth {
public:
    /**
     * The Constructor.
     */
    ColorDepth();
    virtual ~ColorDepth();

    virtual std::string getCategory() const { return "Image Processing"; }
    virtual std::string getClassName() const { return "ColorDepth"; }
    virtual Processor::CodeState getCodeState() const { return CODE_STATE_STABLE; }
    virtual std::string getProcessorInfo() const;
    virtual Processor* create() const { return new ColorDepth(); }

    virtual void initialize() throw (VoreenException);

    void process();

protected:
    tgt::Texture* chromaDepthTex_; ///< The texture used for the chromadepth color coding
    IntOptionProperty colorMode_; ///< The color mode to choose (Light-dark (replace/modulate), chromadepth, pseudo chromadepth)

    FloatProperty factor_;  ///< Controls the influence of the depth values

    RenderPort inport_;
    RenderPort outport_;
};


} // namespace voreen

#endif //VRN_COLORDEPTHPP_H
