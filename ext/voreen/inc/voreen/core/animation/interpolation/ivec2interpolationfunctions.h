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

#ifndef VRN_IVEC2INTERPOLATIONFUNCTIONS_H
#define VRN_IVEC2INTERPOLATIONFUNCTIONS_H

#include "voreen/core/animation/interpolationfunction.h"
#include "tgt/vector.h"

namespace voreen {

/**
 * This class offers an interpolation function for ivec2-values. Interpolation: focus on startvalue.
 */
class IVec2StartInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2StartInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2-values. Interpolation: focus on endvalue.
 */
class IVec2EndInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2EndInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2-values. Interpolation: bisection.
 */
class IVec2StartEndInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2StartEndInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: linear.
 */
class IVec2LinearInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2LinearInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: quadratic (easing in).
 */
class IVec2InQuadInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InQuadInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: cubicular (easing in).
 */
class IVec2InCubicInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InCubicInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: quartetic (easing in).
 */
class IVec2InQuartInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InQuartInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: quintic (easing in).
 */
class IVec2InQuintInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InQuintInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: sineousidal (easing in).
 */
class IVec2InSineInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InSineInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: exponential (easing in).
 */
class IVec2InExponentInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InExponentInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: circular (easing in).
 */
class IVec2InCircInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InCircInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: quadratic (easing out).
 */
class IVec2OutQuadInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutQuadInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: cubicular (easing out).
 */
class IVec2OutCubicInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutCubicInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: quartetic (easing out).
 */
class IVec2OutQuartInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutQuartInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: quintic (easing out).
 */
class IVec2OutQuintInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutQuintInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: sineousidal (easing out).
 */
class IVec2OutSineInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutSineInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: exponential (easing out).
 */
class IVec2OutExponentInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutExponentInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: circular (easing out).
 */
class IVec2OutCircInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutCircInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: quadratic (easing in, then easing out).
 */
class IVec2InOutQuadInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InOutQuadInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: cubicular (easing in, then easing out).
 */
class IVec2InOutCubicInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InOutCubicInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: quartetic (easing in, then easing out).
 */
class IVec2InOutQuartInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InOutQuartInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: quintic (easing in, then easing out).
 */
class IVec2InOutQuintInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InOutQuintInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: sineousidal (easing in, then easing out).
 */
class IVec2InOutSineInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InOutSineInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: exponential (easing in, then easing out).
 */
class IVec2InOutExponentInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InOutExponentInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: circular (easing in, then easing out).
 */
class IVec2InOutCircInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2InOutCircInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: quadratic (easing out, then easing in).
 */
class IVec2OutInQuadInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutInQuadInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: cubicular (easing out, then easing in).
 */
class IVec2OutInCubicInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutInCubicInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: quartetic (easing out, then easing in).
 */
class IVec2OutInQuartInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutInQuartInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: quintic (easing out, then easing in).
 */
class IVec2OutInQuintInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutInQuintInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: sineousidal (easing out, then easing in).
 */
class IVec2OutInSineInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutInSineInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: exponential (easing out, then easing in).
 */
class IVec2OutInExponentInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutInExponentInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

/**
 * This class offers an interpolation function for ivec2. Interpolation: circular (easing out, then easing in).
 */
class IVec2OutInCircInterpolationFunction : public InterpolationFunction<tgt::ivec2> {
public:
    IVec2OutInCircInterpolationFunction();
    InterpolationFunction<tgt::ivec2>* clone() const;
    tgt::ivec2 interpolate(tgt::ivec2 startvalue, tgt::ivec2 endvalue, float time) const;

    std::string getMode() const;
    std::string getIdentifier() const;
};

} // namespace voreen
#endif
