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

#include "voreen/core/animation/interpolation/basicfloatinterpolation.h"
#include "voreen/core/animation/interpolation/vec4interpolationfunctions.h"

namespace voreen {

Vec4StartInterpolationFunction::Vec4StartInterpolationFunction() {}

std::string Vec4StartInterpolationFunction::getMode() const {
    return "focus on startvalue";
}

std::string Vec4StartInterpolationFunction::getIdentifier() const {
    return "boolean";
}

tgt::vec4 Vec4StartInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    if (time < 1.f)
        return startvalue;
    else
        return endvalue;
}

InterpolationFunction<tgt::vec4>* Vec4StartInterpolationFunction::clone() const {
    return new Vec4StartInterpolationFunction();
}

Vec4EndInterpolationFunction::Vec4EndInterpolationFunction() {}

std::string Vec4EndInterpolationFunction::getMode() const {
    return "focus on endvalue";
}

std::string Vec4EndInterpolationFunction::getIdentifier() const {
    return "boolean";
}

tgt::vec4 Vec4EndInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    if (time > 0.f)
        return endvalue;
    else
        return startvalue;
}

InterpolationFunction<tgt::vec4>* Vec4EndInterpolationFunction::clone() const {
    return new Vec4EndInterpolationFunction();
}

Vec4StartEndInterpolationFunction::Vec4StartEndInterpolationFunction() {}

std::string Vec4StartEndInterpolationFunction::getMode() const {
    return "bisection";
}

std::string Vec4StartEndInterpolationFunction::getIdentifier() const {
    return "boolean";
}

tgt::vec4 Vec4StartEndInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    if (time < 0.5f)
        return startvalue;
    else
        return endvalue;
}

InterpolationFunction<tgt::vec4>* Vec4StartEndInterpolationFunction::clone() const {
    return new Vec4StartEndInterpolationFunction();
}

Vec4LinearInterpolationFunction::Vec4LinearInterpolationFunction() {}

std::string Vec4LinearInterpolationFunction::getMode() const {
    return "linear interpolation";
}

std::string Vec4LinearInterpolationFunction::getIdentifier() const {
    return "linear";
}

tgt::vec4 Vec4LinearInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::linearInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::linearInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::linearInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::linearInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4LinearInterpolationFunction::clone() const {
    return new Vec4LinearInterpolationFunction();
}

Vec4InQuadInterpolationFunction::Vec4InQuadInterpolationFunction() {}

std::string Vec4InQuadInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec4InQuadInterpolationFunction::getIdentifier() const {
    return "quadratic";
}

tgt::vec4 Vec4InQuadInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inQuadInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inQuadInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inQuadInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inQuadInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InQuadInterpolationFunction::clone() const {
    return new Vec4InQuadInterpolationFunction();
}

Vec4InCubicInterpolationFunction::Vec4InCubicInterpolationFunction() {}

std::string Vec4InCubicInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec4InCubicInterpolationFunction::getIdentifier() const {
    return "cubic";
}

tgt::vec4 Vec4InCubicInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inCubicInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inCubicInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inCubicInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inCubicInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InCubicInterpolationFunction::clone() const {
    return new Vec4InCubicInterpolationFunction();
}

Vec4InQuartInterpolationFunction::Vec4InQuartInterpolationFunction() {}

std::string Vec4InQuartInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec4InQuartInterpolationFunction::getIdentifier() const {
    return "quartetic";
}

tgt::vec4 Vec4InQuartInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inQuartInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inQuartInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inQuartInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inQuartInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InQuartInterpolationFunction::clone() const {
    return new Vec4InQuartInterpolationFunction();
}

Vec4InQuintInterpolationFunction::Vec4InQuintInterpolationFunction() {}

std::string Vec4InQuintInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec4InQuintInterpolationFunction::getIdentifier() const {
    return "quintic";
}

tgt::vec4 Vec4InQuintInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inQuintInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inQuintInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inQuintInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inQuintInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InQuintInterpolationFunction::clone() const {
    return new Vec4InQuintInterpolationFunction();
}

Vec4InSineInterpolationFunction::Vec4InSineInterpolationFunction() {}

std::string Vec4InSineInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec4InSineInterpolationFunction::getIdentifier() const {
    return "sineousidal";
}

tgt::vec4 Vec4InSineInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inSineInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inSineInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inSineInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inSineInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InSineInterpolationFunction::clone() const {
    return new Vec4InSineInterpolationFunction();
}

Vec4InExponentInterpolationFunction::Vec4InExponentInterpolationFunction() {}

std::string Vec4InExponentInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec4InExponentInterpolationFunction::getIdentifier() const {
    return "exponential";
}

tgt::vec4 Vec4InExponentInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inExponentInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inExponentInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inExponentInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inExponentInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InExponentInterpolationFunction::clone() const {
    return new Vec4InExponentInterpolationFunction();
}

Vec4InCircInterpolationFunction::Vec4InCircInterpolationFunction() {}

std::string Vec4InCircInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec4InCircInterpolationFunction::getIdentifier() const {
    return "circular";
}

tgt::vec4 Vec4InCircInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inCircInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inCircInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inCircInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inCircInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InCircInterpolationFunction::clone() const {
    return new Vec4InCircInterpolationFunction();
}

Vec4OutQuadInterpolationFunction::Vec4OutQuadInterpolationFunction() {}

std::string Vec4OutQuadInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec4OutQuadInterpolationFunction::getIdentifier() const {
    return "quadratic";
}

tgt::vec4 Vec4OutQuadInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outQuadInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outQuadInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outQuadInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outQuadInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutQuadInterpolationFunction::clone() const {
    return new Vec4OutQuadInterpolationFunction();
}

Vec4OutCubicInterpolationFunction::Vec4OutCubicInterpolationFunction() {}

std::string Vec4OutCubicInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec4OutCubicInterpolationFunction::getIdentifier() const {
    return "cubic";
}

tgt::vec4 Vec4OutCubicInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outCubicInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outCubicInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outCubicInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outCubicInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutCubicInterpolationFunction::clone() const {
    return new Vec4OutCubicInterpolationFunction();
}

Vec4OutQuartInterpolationFunction::Vec4OutQuartInterpolationFunction() {}

std::string Vec4OutQuartInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec4OutQuartInterpolationFunction::getIdentifier() const {
    return "quartetic";
}

tgt::vec4 Vec4OutQuartInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outQuartInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outQuartInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outQuartInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outQuartInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutQuartInterpolationFunction::clone() const {
    return new Vec4OutQuartInterpolationFunction();
}

Vec4OutQuintInterpolationFunction::Vec4OutQuintInterpolationFunction() {}

std::string Vec4OutQuintInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec4OutQuintInterpolationFunction::getIdentifier() const {
    return "quintic";
}

tgt::vec4 Vec4OutQuintInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outQuintInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outQuintInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outQuintInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outQuintInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutQuintInterpolationFunction::clone() const {
    return new Vec4OutQuintInterpolationFunction();
}

Vec4OutSineInterpolationFunction::Vec4OutSineInterpolationFunction() {}

std::string Vec4OutSineInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec4OutSineInterpolationFunction::getIdentifier() const {
    return "sineousidal";
}

tgt::vec4 Vec4OutSineInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outSineInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outSineInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outSineInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outSineInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutSineInterpolationFunction::clone() const {
    return new Vec4OutSineInterpolationFunction();
}

Vec4OutExponentInterpolationFunction::Vec4OutExponentInterpolationFunction() {}

std::string Vec4OutExponentInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec4OutExponentInterpolationFunction::getIdentifier() const {
    return "exponential";
}

tgt::vec4 Vec4OutExponentInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outExponentInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outExponentInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outExponentInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outExponentInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutExponentInterpolationFunction::clone() const {
    return new Vec4OutExponentInterpolationFunction();
}

Vec4OutCircInterpolationFunction::Vec4OutCircInterpolationFunction() {}

std::string Vec4OutCircInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec4OutCircInterpolationFunction::getIdentifier() const {
    return "circular";
}

tgt::vec4 Vec4OutCircInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outCircInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outCircInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outCircInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outCircInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutCircInterpolationFunction::clone() const {
    return new Vec4OutCircInterpolationFunction();
}

Vec4InOutQuadInterpolationFunction::Vec4InOutQuadInterpolationFunction() {}

std::string Vec4InOutQuadInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec4InOutQuadInterpolationFunction::getIdentifier() const {
    return "quadratic";
}

tgt::vec4 Vec4InOutQuadInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutQuadInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutQuadInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutQuadInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inOutQuadInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InOutQuadInterpolationFunction::clone() const {
    return new Vec4InOutQuadInterpolationFunction();
}

Vec4InOutCubicInterpolationFunction::Vec4InOutCubicInterpolationFunction() {}

std::string Vec4InOutCubicInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec4InOutCubicInterpolationFunction::getIdentifier() const {
    return "cubic";
}

tgt::vec4 Vec4InOutCubicInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutCubicInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutCubicInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutCubicInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inOutCubicInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InOutCubicInterpolationFunction::clone() const {
    return new Vec4InOutCubicInterpolationFunction();
}

Vec4InOutQuartInterpolationFunction::Vec4InOutQuartInterpolationFunction() {}

std::string Vec4InOutQuartInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec4InOutQuartInterpolationFunction::getIdentifier() const {
    return "quartetic";
}

tgt::vec4 Vec4InOutQuartInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutQuartInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutQuartInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutQuartInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inOutQuartInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InOutQuartInterpolationFunction::clone() const {
    return new Vec4InOutQuartInterpolationFunction();
}

Vec4InOutQuintInterpolationFunction::Vec4InOutQuintInterpolationFunction() {}

std::string Vec4InOutQuintInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec4InOutQuintInterpolationFunction::getIdentifier() const {
    return "quintic";
}

tgt::vec4 Vec4InOutQuintInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutQuintInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutQuintInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutQuintInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inOutQuintInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InOutQuintInterpolationFunction::clone() const {
    return new Vec4InOutQuintInterpolationFunction();
}

Vec4InOutSineInterpolationFunction::Vec4InOutSineInterpolationFunction() {}

std::string Vec4InOutSineInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec4InOutSineInterpolationFunction::getIdentifier() const {
    return "sineousidal";
}

tgt::vec4 Vec4InOutSineInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutSineInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutSineInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutSineInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inOutSineInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InOutSineInterpolationFunction::clone() const {
    return new Vec4InOutSineInterpolationFunction();
}

Vec4InOutExponentInterpolationFunction::Vec4InOutExponentInterpolationFunction() {}

std::string Vec4InOutExponentInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec4InOutExponentInterpolationFunction::getIdentifier() const {
    return "exponential";
}

tgt::vec4 Vec4InOutExponentInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutExponentInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutExponentInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutExponentInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inOutExponentInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InOutExponentInterpolationFunction::clone() const {
    return new Vec4InOutExponentInterpolationFunction();
}

Vec4InOutCircInterpolationFunction::Vec4InOutCircInterpolationFunction() {}

std::string Vec4InOutCircInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec4InOutCircInterpolationFunction::getIdentifier() const {
    return "circular";
}

tgt::vec4 Vec4InOutCircInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutCircInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutCircInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutCircInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::inOutCircInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4InOutCircInterpolationFunction::clone() const {
    return new Vec4InOutCircInterpolationFunction();
}

Vec4OutInQuadInterpolationFunction::Vec4OutInQuadInterpolationFunction() {}

std::string Vec4OutInQuadInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec4OutInQuadInterpolationFunction::getIdentifier() const {
    return "quadratic";
}

tgt::vec4 Vec4OutInQuadInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInQuadInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInQuadInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInQuadInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outInQuadInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutInQuadInterpolationFunction::clone() const {
    return new Vec4OutInQuadInterpolationFunction();
}

Vec4OutInCubicInterpolationFunction::Vec4OutInCubicInterpolationFunction() {}

std::string Vec4OutInCubicInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec4OutInCubicInterpolationFunction::getIdentifier() const {
    return "cubic";
}

tgt::vec4 Vec4OutInCubicInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInCubicInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInCubicInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInCubicInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outInCubicInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutInCubicInterpolationFunction::clone() const {
    return new Vec4OutInCubicInterpolationFunction();
}

Vec4OutInQuartInterpolationFunction::Vec4OutInQuartInterpolationFunction() {}

std::string Vec4OutInQuartInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec4OutInQuartInterpolationFunction::getIdentifier() const {
    return "quartetic";
}

tgt::vec4 Vec4OutInQuartInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInQuartInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInQuartInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInQuartInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outInQuartInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutInQuartInterpolationFunction::clone() const {
    return new Vec4OutInQuartInterpolationFunction();
}

Vec4OutInQuintInterpolationFunction::Vec4OutInQuintInterpolationFunction() {}

std::string Vec4OutInQuintInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec4OutInQuintInterpolationFunction::getIdentifier() const {
    return "quintic";
}

tgt::vec4 Vec4OutInQuintInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInQuintInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInQuintInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInQuintInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outInQuintInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutInQuintInterpolationFunction::clone() const {
    return new Vec4OutInQuintInterpolationFunction();
}

Vec4OutInSineInterpolationFunction::Vec4OutInSineInterpolationFunction() {}

std::string Vec4OutInSineInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec4OutInSineInterpolationFunction::getIdentifier() const {
    return "sineousidal";
}

tgt::vec4 Vec4OutInSineInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInSineInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInSineInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInSineInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outInSineInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutInSineInterpolationFunction::clone() const {
    return new Vec4OutInSineInterpolationFunction();
}

Vec4OutInExponentInterpolationFunction::Vec4OutInExponentInterpolationFunction() {}

std::string Vec4OutInExponentInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec4OutInExponentInterpolationFunction::getIdentifier() const {
    return "exponential";
}

tgt::vec4 Vec4OutInExponentInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInExponentInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInExponentInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInExponentInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outInExponentInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutInExponentInterpolationFunction::clone() const {
    return new Vec4OutInExponentInterpolationFunction();
}

Vec4OutInCircInterpolationFunction::Vec4OutInCircInterpolationFunction() {}

std::string Vec4OutInCircInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec4OutInCircInterpolationFunction::getIdentifier() const {
    return "circular";
}

tgt::vec4 Vec4OutInCircInterpolationFunction::interpolate(tgt::vec4 startvalue, tgt::vec4 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInCircInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInCircInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInCircInterpolation(startvalue.z, endvalue.z, time);
    float compA = BasicFloatInterpolation::outInCircInterpolation(startvalue.a, endvalue.a, time);
    return tgt::vec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::vec4>* Vec4OutInCircInterpolationFunction::clone() const {
    return new Vec4OutInCircInterpolationFunction();
}

} // namespace voreen
