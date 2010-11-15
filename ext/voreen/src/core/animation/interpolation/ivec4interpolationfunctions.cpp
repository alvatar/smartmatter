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

#include "voreen/core/animation/interpolation/basicintinterpolation.h"
#include "voreen/core/animation/interpolation/ivec4interpolationfunctions.h"

namespace voreen {

IVec4StartInterpolationFunction::IVec4StartInterpolationFunction() {}

std::string IVec4StartInterpolationFunction::getMode() const {
    return "focus on startvalue";
}

std::string IVec4StartInterpolationFunction::getIdentifier() const {
    return "boolean";
}

tgt::ivec4 IVec4StartInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    if (time < 1.f)
        return startvalue;
    else
        return endvalue;
}

InterpolationFunction<tgt::ivec4>* IVec4StartInterpolationFunction::clone() const {
    return new IVec4StartInterpolationFunction();
}

IVec4EndInterpolationFunction::IVec4EndInterpolationFunction() {}

std::string IVec4EndInterpolationFunction::getMode() const {
    return "focus on endvalue";
}

std::string IVec4EndInterpolationFunction::getIdentifier() const {
    return "boolean";
}

tgt::ivec4 IVec4EndInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    if (time > 0.f)
        return endvalue;
    else
        return startvalue;
}

InterpolationFunction<tgt::ivec4>* IVec4EndInterpolationFunction::clone() const {
    return new IVec4EndInterpolationFunction();
}

IVec4StartEndInterpolationFunction::IVec4StartEndInterpolationFunction() {}

std::string IVec4StartEndInterpolationFunction::getMode() const {
    return "bisection";
}

std::string IVec4StartEndInterpolationFunction::getIdentifier() const {
    return "boolean";
}

tgt::ivec4 IVec4StartEndInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    if (time < 0.5f)
        return startvalue;
    else
        return endvalue;
}

InterpolationFunction<tgt::ivec4>* IVec4StartEndInterpolationFunction::clone() const {
    return new IVec4StartEndInterpolationFunction();
}

IVec4LinearInterpolationFunction::IVec4LinearInterpolationFunction() {}

std::string IVec4LinearInterpolationFunction::getMode() const {
    return "linear interpolation";
}

std::string IVec4LinearInterpolationFunction::getIdentifier() const {
    return "linear";
}

tgt::ivec4 IVec4LinearInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::linearInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::linearInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::linearInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::linearInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4LinearInterpolationFunction::clone() const {
    return new IVec4LinearInterpolationFunction();
}

IVec4InQuadInterpolationFunction::IVec4InQuadInterpolationFunction() {}

std::string IVec4InQuadInterpolationFunction::getMode() const {
    return "easing in";
}

std::string IVec4InQuadInterpolationFunction::getIdentifier() const {
    return "quadratic";
}

tgt::ivec4 IVec4InQuadInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inQuadInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inQuadInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inQuadInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inQuadInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InQuadInterpolationFunction::clone() const {
    return new IVec4InQuadInterpolationFunction();
}

IVec4InCubicInterpolationFunction::IVec4InCubicInterpolationFunction() {}

std::string IVec4InCubicInterpolationFunction::getMode() const {
    return "easing in";
}

std::string IVec4InCubicInterpolationFunction::getIdentifier() const {
    return "cubic";
}

tgt::ivec4 IVec4InCubicInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inCubicInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inCubicInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inCubicInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inCubicInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InCubicInterpolationFunction::clone() const {
    return new IVec4InCubicInterpolationFunction();
}

IVec4InQuartInterpolationFunction::IVec4InQuartInterpolationFunction() {}

std::string IVec4InQuartInterpolationFunction::getMode() const {
    return "easing in";
}

std::string IVec4InQuartInterpolationFunction::getIdentifier() const {
    return "quartetic";
}

tgt::ivec4 IVec4InQuartInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inQuartInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inQuartInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inQuartInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inQuartInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InQuartInterpolationFunction::clone() const {
    return new IVec4InQuartInterpolationFunction();
}

IVec4InQuintInterpolationFunction::IVec4InQuintInterpolationFunction() {}

std::string IVec4InQuintInterpolationFunction::getMode() const {
    return "easing in";
}

std::string IVec4InQuintInterpolationFunction::getIdentifier() const {
    return "quintic";
}

tgt::ivec4 IVec4InQuintInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inQuintInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inQuintInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inQuintInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inQuintInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InQuintInterpolationFunction::clone() const {
    return new IVec4InQuintInterpolationFunction();
}

IVec4InSineInterpolationFunction::IVec4InSineInterpolationFunction() {}

std::string IVec4InSineInterpolationFunction::getMode() const {
    return "easing in";
}

std::string IVec4InSineInterpolationFunction::getIdentifier() const {
    return "sineousidal";
}

tgt::ivec4 IVec4InSineInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inSineInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inSineInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inSineInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inSineInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InSineInterpolationFunction::clone() const {
    return new IVec4InSineInterpolationFunction();
}

IVec4InExponentInterpolationFunction::IVec4InExponentInterpolationFunction() {}

std::string IVec4InExponentInterpolationFunction::getMode() const {
    return "easing in";
}

std::string IVec4InExponentInterpolationFunction::getIdentifier() const {
    return "exponential";
}

tgt::ivec4 IVec4InExponentInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inExponentInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inExponentInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inExponentInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inExponentInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InExponentInterpolationFunction::clone() const {
    return new IVec4InExponentInterpolationFunction();
}

IVec4InCircInterpolationFunction::IVec4InCircInterpolationFunction() {}

std::string IVec4InCircInterpolationFunction::getMode() const {
    return "easing in";
}

std::string IVec4InCircInterpolationFunction::getIdentifier() const {
    return "circular";
}

tgt::ivec4 IVec4InCircInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inCircInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inCircInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inCircInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inCircInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InCircInterpolationFunction::clone() const {
    return new IVec4InCircInterpolationFunction();
}

IVec4OutQuadInterpolationFunction::IVec4OutQuadInterpolationFunction() {}

std::string IVec4OutQuadInterpolationFunction::getMode() const {
    return "easing out";
}

std::string IVec4OutQuadInterpolationFunction::getIdentifier() const {
    return "quadratic";
}

tgt::ivec4 IVec4OutQuadInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outQuadInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outQuadInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outQuadInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outQuadInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutQuadInterpolationFunction::clone() const {
    return new IVec4OutQuadInterpolationFunction();
}

IVec4OutCubicInterpolationFunction::IVec4OutCubicInterpolationFunction() {}

std::string IVec4OutCubicInterpolationFunction::getMode() const {
    return "easing out";
}

std::string IVec4OutCubicInterpolationFunction::getIdentifier() const {
    return "cubic";
}

tgt::ivec4 IVec4OutCubicInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outCubicInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outCubicInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outCubicInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outCubicInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutCubicInterpolationFunction::clone() const {
    return new IVec4OutCubicInterpolationFunction();
}

IVec4OutQuartInterpolationFunction::IVec4OutQuartInterpolationFunction() {}

std::string IVec4OutQuartInterpolationFunction::getMode() const {
    return "easing out";
}

std::string IVec4OutQuartInterpolationFunction::getIdentifier() const {
    return "quartetic";
}

tgt::ivec4 IVec4OutQuartInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outQuartInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outQuartInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outQuartInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outQuartInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutQuartInterpolationFunction::clone() const {
    return new IVec4OutQuartInterpolationFunction();
}

IVec4OutQuintInterpolationFunction::IVec4OutQuintInterpolationFunction() {}

std::string IVec4OutQuintInterpolationFunction::getMode() const {
    return "easing out";
}

std::string IVec4OutQuintInterpolationFunction::getIdentifier() const {
    return "quintic";
}

tgt::ivec4 IVec4OutQuintInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outQuintInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outQuintInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outQuintInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outQuintInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutQuintInterpolationFunction::clone() const {
    return new IVec4OutQuintInterpolationFunction();
}

IVec4OutSineInterpolationFunction::IVec4OutSineInterpolationFunction() {}

std::string IVec4OutSineInterpolationFunction::getMode() const {
    return "easing out";
}

std::string IVec4OutSineInterpolationFunction::getIdentifier() const {
    return "sineousidal";
}

tgt::ivec4 IVec4OutSineInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outSineInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outSineInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outSineInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outSineInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutSineInterpolationFunction::clone() const {
    return new IVec4OutSineInterpolationFunction();
}

IVec4OutExponentInterpolationFunction::IVec4OutExponentInterpolationFunction() {}

std::string IVec4OutExponentInterpolationFunction::getMode() const {
    return "easing out";
}

std::string IVec4OutExponentInterpolationFunction::getIdentifier() const {
    return "exponential";
}

tgt::ivec4 IVec4OutExponentInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outExponentInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outExponentInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outExponentInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outExponentInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutExponentInterpolationFunction::clone() const {
    return new IVec4OutExponentInterpolationFunction();
}

IVec4OutCircInterpolationFunction::IVec4OutCircInterpolationFunction() {}

std::string IVec4OutCircInterpolationFunction::getMode() const {
    return "easing out";
}

std::string IVec4OutCircInterpolationFunction::getIdentifier() const {
    return "circular";
}

tgt::ivec4 IVec4OutCircInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outCircInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outCircInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outCircInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outCircInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutCircInterpolationFunction::clone() const {
    return new IVec4OutCircInterpolationFunction();
}

IVec4InOutQuadInterpolationFunction::IVec4InOutQuadInterpolationFunction() {}

std::string IVec4InOutQuadInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string IVec4InOutQuadInterpolationFunction::getIdentifier() const {
    return "quadratic";
}

tgt::ivec4 IVec4InOutQuadInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inOutQuadInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inOutQuadInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inOutQuadInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inOutQuadInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InOutQuadInterpolationFunction::clone() const {
    return new IVec4InOutQuadInterpolationFunction();
}

IVec4InOutCubicInterpolationFunction::IVec4InOutCubicInterpolationFunction() {}

std::string IVec4InOutCubicInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string IVec4InOutCubicInterpolationFunction::getIdentifier() const {
    return "cubic";
}

tgt::ivec4 IVec4InOutCubicInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inOutCubicInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inOutCubicInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inOutCubicInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inOutCubicInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InOutCubicInterpolationFunction::clone() const {
    return new IVec4InOutCubicInterpolationFunction();
}

IVec4InOutQuartInterpolationFunction::IVec4InOutQuartInterpolationFunction() {}

std::string IVec4InOutQuartInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string IVec4InOutQuartInterpolationFunction::getIdentifier() const {
    return "quartetic";
}

tgt::ivec4 IVec4InOutQuartInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inOutQuartInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inOutQuartInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inOutQuartInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inOutQuartInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InOutQuartInterpolationFunction::clone() const {
    return new IVec4InOutQuartInterpolationFunction();
}

IVec4InOutQuintInterpolationFunction::IVec4InOutQuintInterpolationFunction() {}

std::string IVec4InOutQuintInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string IVec4InOutQuintInterpolationFunction::getIdentifier() const {
    return "quintic";
}

tgt::ivec4 IVec4InOutQuintInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inOutQuintInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inOutQuintInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inOutQuintInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inOutQuintInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InOutQuintInterpolationFunction::clone() const {
    return new IVec4InOutQuintInterpolationFunction();
}

IVec4InOutSineInterpolationFunction::IVec4InOutSineInterpolationFunction() {}

std::string IVec4InOutSineInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string IVec4InOutSineInterpolationFunction::getIdentifier() const {
    return "sineousidal";
}

tgt::ivec4 IVec4InOutSineInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inOutSineInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inOutSineInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inOutSineInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inOutSineInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InOutSineInterpolationFunction::clone() const {
    return new IVec4InOutSineInterpolationFunction();
}

IVec4InOutExponentInterpolationFunction::IVec4InOutExponentInterpolationFunction() {}

std::string IVec4InOutExponentInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string IVec4InOutExponentInterpolationFunction::getIdentifier() const {
    return "exponential";
}

tgt::ivec4 IVec4InOutExponentInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inOutExponentInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inOutExponentInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inOutExponentInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inOutExponentInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InOutExponentInterpolationFunction::clone() const {
    return new IVec4InOutExponentInterpolationFunction();
}

IVec4InOutCircInterpolationFunction::IVec4InOutCircInterpolationFunction() {}

std::string IVec4InOutCircInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string IVec4InOutCircInterpolationFunction::getIdentifier() const {
    return "circular";
}

tgt::ivec4 IVec4InOutCircInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::inOutCircInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::inOutCircInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::inOutCircInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::inOutCircInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4InOutCircInterpolationFunction::clone() const {
    return new IVec4InOutCircInterpolationFunction();
}

IVec4OutInQuadInterpolationFunction::IVec4OutInQuadInterpolationFunction() {}

std::string IVec4OutInQuadInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string IVec4OutInQuadInterpolationFunction::getIdentifier() const {
    return "quadratic";
}

tgt::ivec4 IVec4OutInQuadInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outInQuadInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outInQuadInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outInQuadInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outInQuadInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutInQuadInterpolationFunction::clone() const {
    return new IVec4OutInQuadInterpolationFunction();
}

IVec4OutInCubicInterpolationFunction::IVec4OutInCubicInterpolationFunction() {}

std::string IVec4OutInCubicInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string IVec4OutInCubicInterpolationFunction::getIdentifier() const {
    return "cubic";
}

tgt::ivec4 IVec4OutInCubicInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outInCubicInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outInCubicInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outInCubicInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outInCubicInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutInCubicInterpolationFunction::clone() const {
    return new IVec4OutInCubicInterpolationFunction();
}

IVec4OutInQuartInterpolationFunction::IVec4OutInQuartInterpolationFunction() {}

std::string IVec4OutInQuartInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string IVec4OutInQuartInterpolationFunction::getIdentifier() const {
    return "quartetic";
}

tgt::ivec4 IVec4OutInQuartInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outInQuartInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outInQuartInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outInQuartInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outInQuartInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutInQuartInterpolationFunction::clone() const {
    return new IVec4OutInQuartInterpolationFunction();
}

IVec4OutInQuintInterpolationFunction::IVec4OutInQuintInterpolationFunction() {}

std::string IVec4OutInQuintInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string IVec4OutInQuintInterpolationFunction::getIdentifier() const {
    return "quintic";
}

tgt::ivec4 IVec4OutInQuintInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outInQuintInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outInQuintInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outInQuintInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outInQuintInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutInQuintInterpolationFunction::clone() const {
    return new IVec4OutInQuintInterpolationFunction();
}

IVec4OutInSineInterpolationFunction::IVec4OutInSineInterpolationFunction() {}

std::string IVec4OutInSineInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string IVec4OutInSineInterpolationFunction::getIdentifier() const {
    return "sineousidal";
}

tgt::ivec4 IVec4OutInSineInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outInSineInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outInSineInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outInSineInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outInSineInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutInSineInterpolationFunction::clone() const {
    return new IVec4OutInSineInterpolationFunction();
}

IVec4OutInExponentInterpolationFunction::IVec4OutInExponentInterpolationFunction() {}

std::string IVec4OutInExponentInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string IVec4OutInExponentInterpolationFunction::getIdentifier() const {
    return "exponential";
}

tgt::ivec4 IVec4OutInExponentInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outInExponentInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outInExponentInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outInExponentInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outInExponentInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutInExponentInterpolationFunction::clone() const {
    return new IVec4OutInExponentInterpolationFunction();
}

IVec4OutInCircInterpolationFunction::IVec4OutInCircInterpolationFunction() {}

std::string IVec4OutInCircInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string IVec4OutInCircInterpolationFunction::getIdentifier() const {
    return "circular";
}

tgt::ivec4 IVec4OutInCircInterpolationFunction::interpolate(tgt::ivec4 startvalue, tgt::ivec4 endvalue, float time) const {
    int compX = BasicIntInterpolation::outInCircInterpolation(static_cast<float>(startvalue.x), static_cast<float>(endvalue.x), time);
    int compY = BasicIntInterpolation::outInCircInterpolation(static_cast<float>(startvalue.y), static_cast<float>(endvalue.y), time);
    int compZ = BasicIntInterpolation::outInCircInterpolation(static_cast<float>(startvalue.z), static_cast<float>(endvalue.z), time);
    int compA = BasicIntInterpolation::outInCircInterpolation(static_cast<float>(startvalue.a), static_cast<float>(endvalue.a), time);
    return tgt::ivec4(compX, compY, compZ, compA);
}

InterpolationFunction<tgt::ivec4>* IVec4OutInCircInterpolationFunction::clone() const {
    return new IVec4OutInCircInterpolationFunction();
}

} // namespace voreen
