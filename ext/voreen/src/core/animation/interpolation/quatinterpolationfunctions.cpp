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

#include "voreen/core/animation/interpolation/quatinterpolationfunctions.h"

namespace voreen {

QuatStartInterpolationFunction::QuatStartInterpolationFunction() {}

std::string QuatStartInterpolationFunction::getMode() const {
    return "focus on startvalue";
}

std::string QuatStartInterpolationFunction::getIdentifier() const {
    return "boolean";
}

tgt::quat QuatStartInterpolationFunction::interpolate(tgt::quat startvalue, tgt::quat endvalue, float time) const {
    if (time < 1.f)
        return startvalue;
    else
        return endvalue;
}

InterpolationFunction<tgt::quat>* QuatStartInterpolationFunction::clone() const {
    return new QuatStartInterpolationFunction();
}

QuatEndInterpolationFunction::QuatEndInterpolationFunction() {}

std::string QuatEndInterpolationFunction::getMode() const {
    return "focus on endvalue";
}

std::string QuatEndInterpolationFunction::getIdentifier() const {
    return "boolean";
}

tgt::quat QuatEndInterpolationFunction::interpolate(tgt::quat startvalue, tgt::quat endvalue, float time) const {
    if (time > 0.f)
        return endvalue;
    else
        return startvalue;
}

InterpolationFunction<tgt::quat>* QuatEndInterpolationFunction::clone() const {
        return new QuatEndInterpolationFunction();
}

QuatStartEndInterpolationFunction::QuatStartEndInterpolationFunction() {}

std::string QuatStartEndInterpolationFunction::getMode() const {
    return "bisection";
}

std::string QuatStartEndInterpolationFunction::getIdentifier() const {
    return "boolean";
}

tgt::quat QuatStartEndInterpolationFunction::interpolate(tgt::quat startvalue, tgt::quat endvalue, float time) const {
    if (time < 0.5f)
        return startvalue;
    else
        return endvalue;
}

InterpolationFunction<tgt::quat>* QuatStartEndInterpolationFunction::clone() const {
    return new QuatStartEndInterpolationFunction();
}

QuatLinearInterpolationFunction::QuatLinearInterpolationFunction() {}

std::string QuatLinearInterpolationFunction::getMode() const {
    return "linear interpolation (Lerp)";
}

std::string QuatLinearInterpolationFunction::getIdentifier() const {
    return "linear";
}

tgt::quat QuatLinearInterpolationFunction::interpolate(tgt::quat startvalue, tgt::quat endvalue, float time) const {
    return lerpQuat(startvalue, endvalue, time);
}

InterpolationFunction<tgt::quat>* QuatLinearInterpolationFunction::clone() const {
    return new QuatLinearInterpolationFunction();
}

QuatSphericalLinearInterpolationFunction::QuatSphericalLinearInterpolationFunction() {}

std::string QuatSphericalLinearInterpolationFunction::getMode() const {
    return "spherical linear interpolation (Slerp)";
}

std::string QuatSphericalLinearInterpolationFunction::getIdentifier() const {
    return "linear";
}

tgt::quat QuatSphericalLinearInterpolationFunction::interpolate(tgt::quat startvalue, tgt::quat endvalue, float time) const {
    return slerpQuat(startvalue, endvalue, time, false);
}

InterpolationFunction<tgt::quat>* QuatSphericalLinearInterpolationFunction::clone() const {
    return new QuatSphericalLinearInterpolationFunction();
}

} // namespace voreen
