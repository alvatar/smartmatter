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

#ifndef VRN_IMAGETHRESHOLD_H
#define VRN_IMAGETHRESHOLD_H

#include "voreen/core/processors/imageprocessor.h"

#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/vectorproperty.h"

namespace voreen {

/**
 * Performs a thresholding.
 *
 * All pixels with intensities below or above the two thresholds
 * are assigned the mask color. Pixels with intensities between
 * the thresholds are passed through.
 */
class ImageThreshold : public ImageProcessor {
public:
    ImageThreshold();
    virtual Processor* create() const;

    virtual std::string getCategory() const   { return "Image Processing"; }
    virtual std::string getClassName() const  { return "ImageThreshold";   }
    virtual CodeState getCodeState() const    { return CODE_STATE_STABLE;  }
    virtual std::string getProcessorInfo() const;

protected:
    void process();

    RenderPort inport_;
    RenderPort outport_;

    FloatProperty lowerThreshold_;  ///< The lower intensity threshold
    FloatProperty upperThreshold_;  ///< The upper intensity threshold
    FloatVec4Property lowerMaskColor_;  ///< Color to assign to pixels below the lower threshold
    FloatVec4Property upperMaskColor_;  ///< Color to assign to pixels above the upper threshold
};


} // namespace voreen

#endif //VRN_IMAGETHRESHOLD_H
