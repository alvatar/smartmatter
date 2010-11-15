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

#ifndef VRN_QUATINTERPOLATIONFUNCTIONS_H
#define VRN_QUATINTERPOLATIONFUNCTIONS_H

#include "voreen/core/animation/interpolationfunction.h"
#include "tgt/quaternion.h"

namespace voreen {

/**
 * This class offers an interpolation function for quat-values. Interpolation: focus on startvalue.
 */
class QuatStartInterpolationFunction : public InterpolationFunction<tgt::quat> {
public:
        QuatStartInterpolationFunction();
        InterpolationFunction<tgt::quat>* clone() const;
        tgt::quat interpolate(tgt::quat startvalue, tgt::quat endvalue, float time) const;

        std::string getMode() const;
        std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for quat-values. Interpolation: focus on endvalue.
 */
class QuatEndInterpolationFunction : public InterpolationFunction<tgt::quat> {
public:
    QuatEndInterpolationFunction();
    InterpolationFunction<tgt::quat>* clone() const;
    tgt::quat interpolate(tgt::quat startvalue, tgt::quat endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for quat-values. Interpolation: bisection.
 */
class QuatStartEndInterpolationFunction : public InterpolationFunction<tgt::quat> {
public:
    QuatStartEndInterpolationFunction();
    InterpolationFunction<tgt::quat>* clone() const;
    tgt::quat interpolate(tgt::quat startvalue, tgt::quat endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for quat. Interpolation: linear.
 */
class QuatLinearInterpolationFunction : public InterpolationFunction<tgt::quat> {
public:
    QuatLinearInterpolationFunction();
    InterpolationFunction<tgt::quat>* clone() const;
    tgt::quat interpolate(tgt::quat startvalue, tgt::quat endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for quat. Interpolation: spherical linear.
 */
class QuatSphericalLinearInterpolationFunction : public InterpolationFunction<tgt::quat> {
public:
    QuatSphericalLinearInterpolationFunction();
    InterpolationFunction<tgt::quat>* clone() const;
    tgt::quat interpolate(tgt::quat startvalue, tgt::quat endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

} //namespace voreen

#endif
