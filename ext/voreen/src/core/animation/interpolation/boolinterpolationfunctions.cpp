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

#include "voreen/core/animation/interpolation/boolinterpolationfunctions.h"

namespace voreen {

BoolStartInterpolationFunction::BoolStartInterpolationFunction() {}

std::string BoolStartInterpolationFunction::getMode() const {
    return "focus on startvalue";
}

std::string BoolStartInterpolationFunction::getIdentifier() const {
    return "boolean";
}

bool BoolStartInterpolationFunction::interpolate(bool startvalue, bool endvalue, float time) const {
    if (time < 1.f)
        return startvalue;
    else
        return endvalue;
}
InterpolationFunction<bool>* BoolStartInterpolationFunction::clone() const {
    return new BoolStartInterpolationFunction();
}

BoolEndInterpolationFunction::BoolEndInterpolationFunction() {}

std::string BoolEndInterpolationFunction::getMode() const {
    return "focus on endvalue";
}

std::string BoolEndInterpolationFunction::getIdentifier() const {
    return "boolean";
}

bool BoolEndInterpolationFunction::interpolate(bool startvalue, bool endvalue, float time) const {
    if (time > 0)
        return endvalue;
    else
        return startvalue;
}
InterpolationFunction<bool>* BoolEndInterpolationFunction::clone() const {
    return new BoolEndInterpolationFunction();
}

BoolStartEndInterpolationFunction::BoolStartEndInterpolationFunction() {}

std::string BoolStartEndInterpolationFunction::getMode() const {
    return "bisection";
}

std::string BoolStartEndInterpolationFunction::getIdentifier() const {
    return "boolean";
}

bool BoolStartEndInterpolationFunction::interpolate(bool startvalue, bool endvalue, float time) const {
    if (time < 0.5f)
        return startvalue;
    else
        return endvalue;
}

InterpolationFunction<bool>* BoolStartEndInterpolationFunction::clone() const {
    return new BoolStartEndInterpolationFunction();
}

} // namespace voreen
