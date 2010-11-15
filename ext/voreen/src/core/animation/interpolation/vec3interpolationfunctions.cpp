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
#include "voreen/core/animation/interpolation/vec3interpolationfunctions.h"
#include "voreen/core/animation/interpolation/floatinterpolationfunctions.h"
#include "tgt/quaternion.h"

namespace voreen {

Vec3StartInterpolationFunction::Vec3StartInterpolationFunction() {}

std::string Vec3StartInterpolationFunction::getMode() const {
    return "focus on startvalue";
}

std::string Vec3StartInterpolationFunction::getIdentifier() const {
    return "boolean";
}

tgt::vec3 Vec3StartInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    if (time < 1.f)
        return startvalue;
    else
        return endvalue;
}

InterpolationFunction<tgt::vec3>* Vec3StartInterpolationFunction::clone() const {
    return new Vec3StartInterpolationFunction();
}

Vec3EndInterpolationFunction::Vec3EndInterpolationFunction() {}

std::string Vec3EndInterpolationFunction::getMode() const {
    return "focus on endvalue";
}

std::string Vec3EndInterpolationFunction::getIdentifier() const {
    return "boolean";
}

tgt::vec3 Vec3EndInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    if (time > 0.f)
        return endvalue;
    else
        return startvalue;
}

InterpolationFunction<tgt::vec3>* Vec3EndInterpolationFunction::clone() const {
    return new Vec3EndInterpolationFunction();
}

Vec3StartEndInterpolationFunction::Vec3StartEndInterpolationFunction() {}

std::string Vec3StartEndInterpolationFunction::getMode() const {
    return "bisection";
}

std::string Vec3StartEndInterpolationFunction::getIdentifier() const {
    return "boolean";
}

tgt::vec3 Vec3StartEndInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    if (time < 0.5f)
        return startvalue;
    else
        return endvalue;
}

InterpolationFunction<tgt::vec3>* Vec3StartEndInterpolationFunction::clone() const {
    return new Vec3StartEndInterpolationFunction();
}

Vec3LinearInterpolationFunction::Vec3LinearInterpolationFunction() {}

std::string Vec3LinearInterpolationFunction::getMode() const {
    return "linear interpolation";
}

std::string Vec3LinearInterpolationFunction::getIdentifier() const {
    return "linear";
}

tgt::vec3 Vec3LinearInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::linearInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::linearInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::linearInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3LinearInterpolationFunction::clone() const {
    return new Vec3LinearInterpolationFunction();
}

Vec3SphericalLinearInterpolationFunction::Vec3SphericalLinearInterpolationFunction() {}

std::string Vec3SphericalLinearInterpolationFunction::getMode() const {
        return "spherical linear interpolation";
}

std::string Vec3SphericalLinearInterpolationFunction::getIdentifier() const {
        return "linear";
}

tgt::vec3 Vec3SphericalLinearInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float eps = 0.001f;
    float dotVal = dot(normalize(startvalue), normalize(endvalue));
    if (dotVal > 0.995f){
        // small angle => linear interpolation
        float compX = BasicFloatInterpolation::linearInterpolation(startvalue.x, endvalue.x, time);
        float compY = BasicFloatInterpolation::linearInterpolation(startvalue.y, endvalue.y, time);
        float compZ = BasicFloatInterpolation::linearInterpolation(startvalue.z, endvalue.z, time);
        return tgt::vec3(compX, compY, compZ);
    }
    float theta = std::acos(dotVal);
    float compX = (sin((1.f - time) * theta)/(sin(theta) + eps)) * startvalue.x
                  + (sin(time * theta)/(sin(theta) + eps)) * endvalue.x;
    float compY = (sin((1.f - time) * theta)/(sin(theta) + eps)) * startvalue.y
                  + (sin(time * theta)/(sin(theta) + eps)) * endvalue.y;
    float compZ = (sin((1.f - time) * theta)/(sin(theta) + eps)) * startvalue.z
                  + (sin(time * theta)/(sin(theta) + eps)) * endvalue.z;
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3SphericalLinearInterpolationFunction::clone() const {
        return new Vec3SphericalLinearInterpolationFunction();
}

Vec3SphericalCubicInterpolationFunction::Vec3SphericalCubicInterpolationFunction() {}

std::string Vec3SphericalCubicInterpolationFunction::getMode() const {
        return "spherical cubic interpolation";
}

std::string Vec3SphericalCubicInterpolationFunction::getIdentifier() const {
        return "spline";
}

tgt::vec3 Vec3SphericalCubicInterpolationFunction::interpolate(std::vector<PropertyKeyValue<tgt::vec3>*> controlpoints, float time) const {
    tgt::quat q0;
    tgt::quat q1;
    tgt::quat q2;
    tgt::quat q3;
    std::vector<PropertyKeyValue<tgt::vec3>*>::iterator it;
    it = controlpoints.begin();
    while ((*it)->getTime() < time)
        it++;

    float time2 = (*it)->getTime();
    q2 = tgt::quat(normalize((*it)->getValue()), 0.0f);
    it--;
    float time1 = (*it)->getTime();
    q1 = tgt::quat(normalize((*it)->getValue()), 0.0f);
    if (it != controlpoints.begin()) {
        it--;
        q0 = tgt::quat(normalize((*it)->getValue()), 0.0f);
        it++;
    } else
        q0 = q1;

    it++;
    it++;
    if (it!= controlpoints.end())
        q3 = tgt::quat(normalize((*it)->getValue()), 0.0f);
    else
        q3 = q2;

    float t = (time - time1)/(time2 - time1);

    tgt::quat qh1 = splineQuat(q0, q1, q2);
    tgt::quat qh2 = splineQuat(q1, q2, q3);
    tgt::quat pos = squadQuat(q1, q2, qh1, qh2, t);
    tgt::vec3 position;
    float length1 = length(q1.vec());
    float length2 = length(q2.vec());
    float lengthRes = length1 * (1-t)+ length2 *t;
    position = pos.vec() * lengthRes;
    return position;
}

MultiPointInterpolationFunction<tgt::vec3>* Vec3SphericalCubicInterpolationFunction::clone() const {
    return new Vec3SphericalCubicInterpolationFunction();
}

Vec3CatmullRomInterpolationFunction::Vec3CatmullRomInterpolationFunction() {}

std::string Vec3CatmullRomInterpolationFunction::getMode() const {
        return "CatmullRom interpolation";
}

std::string Vec3CatmullRomInterpolationFunction::getIdentifier() const {
        return "spline";
}

tgt::vec3 Vec3CatmullRomInterpolationFunction::interpolate(std::vector<PropertyKeyValue<tgt::vec3>*> controlpoints, float time) const {
    FloatCatmullRomInterpolationFunction* intfunc = new FloatCatmullRomInterpolationFunction();
    std::vector<PropertyKeyValue<tgt::vec3>*>::iterator it;
    std::vector<PropertyKeyValue<float>*> xControlPoints;
    std::vector<PropertyKeyValue<float>*> yControlPoints;
    std::vector<PropertyKeyValue<float>*> zControlPoints;
    for (it = controlpoints.begin(); it != controlpoints.end(); ++it) {
        xControlPoints.push_back(new PropertyKeyValue<float>((*it)->getValue().x, (*it)->getTime()));
        yControlPoints.push_back(new PropertyKeyValue<float>((*it)->getValue().y, (*it)->getTime()));
        zControlPoints.push_back(new PropertyKeyValue<float>((*it)->getValue().z, (*it)->getTime()));
    }
    tgt::vec3 ret;
    ret.x = intfunc->interpolate(xControlPoints, time);
    ret.y = intfunc->interpolate(yControlPoints, time);
    ret.z = intfunc->interpolate(zControlPoints, time);
    return ret;
}

MultiPointInterpolationFunction<tgt::vec3>* Vec3CatmullRomInterpolationFunction::clone() const {
        return new Vec3CatmullRomInterpolationFunction();
}

Vec3InQuadInterpolationFunction::Vec3InQuadInterpolationFunction() {}

std::string Vec3InQuadInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec3InQuadInterpolationFunction::getIdentifier() const {
    return "quadratic";
}

tgt::vec3 Vec3InQuadInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inQuadInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inQuadInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inQuadInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InQuadInterpolationFunction::clone() const {
    return new Vec3InQuadInterpolationFunction();
}

Vec3InCubicInterpolationFunction::Vec3InCubicInterpolationFunction() {}

std::string Vec3InCubicInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec3InCubicInterpolationFunction::getIdentifier() const {
    return "cubic";
}

tgt::vec3 Vec3InCubicInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inCubicInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inCubicInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inCubicInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InCubicInterpolationFunction::clone() const {
    return new Vec3InCubicInterpolationFunction();
}

Vec3InQuartInterpolationFunction::Vec3InQuartInterpolationFunction() {}

std::string Vec3InQuartInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec3InQuartInterpolationFunction::getIdentifier() const {
    return "quartetic";
}

tgt::vec3 Vec3InQuartInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inQuartInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inQuartInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inQuartInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InQuartInterpolationFunction::clone() const {
    return new Vec3InQuartInterpolationFunction();
}

Vec3InQuintInterpolationFunction::Vec3InQuintInterpolationFunction() {}

std::string Vec3InQuintInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec3InQuintInterpolationFunction::getIdentifier() const {
    return "quintic";
}

tgt::vec3 Vec3InQuintInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inQuintInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inQuintInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inQuintInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InQuintInterpolationFunction::clone() const {
    return new Vec3InQuintInterpolationFunction();
}

Vec3InSineInterpolationFunction::Vec3InSineInterpolationFunction() {}

std::string Vec3InSineInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec3InSineInterpolationFunction::getIdentifier() const {
    return "sineousidal";
}

tgt::vec3 Vec3InSineInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inSineInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inSineInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inSineInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InSineInterpolationFunction::clone() const {
    return new Vec3InSineInterpolationFunction();
}

Vec3InExponentInterpolationFunction::Vec3InExponentInterpolationFunction() {}

std::string Vec3InExponentInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec3InExponentInterpolationFunction::getIdentifier() const {
    return "exponential";
}

tgt::vec3 Vec3InExponentInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inExponentInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inExponentInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inExponentInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InExponentInterpolationFunction::clone() const {
    return new Vec3InExponentInterpolationFunction();
}

Vec3InCircInterpolationFunction::Vec3InCircInterpolationFunction() {}

std::string Vec3InCircInterpolationFunction::getMode() const {
    return "easing in";
}

std::string Vec3InCircInterpolationFunction::getIdentifier() const {
    return "circular";
}

tgt::vec3 Vec3InCircInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inCircInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inCircInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inCircInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InCircInterpolationFunction::clone() const {
    return new Vec3InCircInterpolationFunction();
}

Vec3OutQuadInterpolationFunction::Vec3OutQuadInterpolationFunction() {}

std::string Vec3OutQuadInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec3OutQuadInterpolationFunction::getIdentifier() const {
    return "quadratic";
}

tgt::vec3 Vec3OutQuadInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outQuadInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outQuadInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outQuadInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutQuadInterpolationFunction::clone() const {
    return new Vec3OutQuadInterpolationFunction();
}

Vec3OutCubicInterpolationFunction::Vec3OutCubicInterpolationFunction() {}

std::string Vec3OutCubicInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec3OutCubicInterpolationFunction::getIdentifier() const {
    return "cubic";
}

tgt::vec3 Vec3OutCubicInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outCubicInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outCubicInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outCubicInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutCubicInterpolationFunction::clone() const {
    return new Vec3OutCubicInterpolationFunction();
}

Vec3OutQuartInterpolationFunction::Vec3OutQuartInterpolationFunction() {}

std::string Vec3OutQuartInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec3OutQuartInterpolationFunction::getIdentifier() const {
    return "quartetic";
}

tgt::vec3 Vec3OutQuartInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outQuartInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outQuartInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outQuartInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutQuartInterpolationFunction::clone() const {
    return new Vec3OutQuartInterpolationFunction();
}

Vec3OutQuintInterpolationFunction::Vec3OutQuintInterpolationFunction() {}

std::string Vec3OutQuintInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec3OutQuintInterpolationFunction::getIdentifier() const {
    return "quintic";
}

tgt::vec3 Vec3OutQuintInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outQuintInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outQuintInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outQuintInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutQuintInterpolationFunction::clone() const {
    return new Vec3OutQuintInterpolationFunction();
}

Vec3OutSineInterpolationFunction::Vec3OutSineInterpolationFunction() {}

std::string Vec3OutSineInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec3OutSineInterpolationFunction::getIdentifier() const {
    return "sineousidal";
}

tgt::vec3 Vec3OutSineInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outSineInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outSineInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outSineInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutSineInterpolationFunction::clone() const {
    return new Vec3OutSineInterpolationFunction();
}

Vec3OutExponentInterpolationFunction::Vec3OutExponentInterpolationFunction() {}

std::string Vec3OutExponentInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec3OutExponentInterpolationFunction::getIdentifier() const {
    return "exponential";
}

tgt::vec3 Vec3OutExponentInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outExponentInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outExponentInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outExponentInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutExponentInterpolationFunction::clone() const {
    return new Vec3OutExponentInterpolationFunction();
}

Vec3OutCircInterpolationFunction::Vec3OutCircInterpolationFunction() {}

std::string Vec3OutCircInterpolationFunction::getMode() const {
    return "easing out";
}

std::string Vec3OutCircInterpolationFunction::getIdentifier() const {
    return "circular";
}

tgt::vec3 Vec3OutCircInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outCircInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outCircInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outCircInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutCircInterpolationFunction::clone() const {
    return new Vec3OutCircInterpolationFunction();
}

Vec3InOutQuadInterpolationFunction::Vec3InOutQuadInterpolationFunction() {}

std::string Vec3InOutQuadInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec3InOutQuadInterpolationFunction::getIdentifier() const {
    return "quadratic";
}

tgt::vec3 Vec3InOutQuadInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutQuadInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutQuadInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutQuadInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InOutQuadInterpolationFunction::clone() const {
    return new Vec3InOutQuadInterpolationFunction();
}

Vec3InOutCubicInterpolationFunction::Vec3InOutCubicInterpolationFunction() {}

std::string Vec3InOutCubicInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec3InOutCubicInterpolationFunction::getIdentifier() const {
    return "cubic";
}

tgt::vec3 Vec3InOutCubicInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutCubicInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutCubicInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutCubicInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InOutCubicInterpolationFunction::clone() const {
    return new Vec3InOutCubicInterpolationFunction();
}

Vec3InOutQuartInterpolationFunction::Vec3InOutQuartInterpolationFunction() {}

std::string Vec3InOutQuartInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec3InOutQuartInterpolationFunction::getIdentifier() const {
    return "quartetic";
}

tgt::vec3 Vec3InOutQuartInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutQuartInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutQuartInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutQuartInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InOutQuartInterpolationFunction::clone() const {
    return new Vec3InOutQuartInterpolationFunction();
}

Vec3InOutQuintInterpolationFunction::Vec3InOutQuintInterpolationFunction() {}

std::string Vec3InOutQuintInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec3InOutQuintInterpolationFunction::getIdentifier() const {
    return "quintic";
}

tgt::vec3 Vec3InOutQuintInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutQuintInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutQuintInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutQuintInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InOutQuintInterpolationFunction::clone() const {
    return new Vec3InOutQuintInterpolationFunction();
}

Vec3InOutSineInterpolationFunction::Vec3InOutSineInterpolationFunction() {}

std::string Vec3InOutSineInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec3InOutSineInterpolationFunction::getIdentifier() const {
    return "sineousidal";
}

tgt::vec3 Vec3InOutSineInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutSineInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutSineInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutSineInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InOutSineInterpolationFunction::clone() const {
    return new Vec3InOutSineInterpolationFunction();
}

Vec3InOutExponentInterpolationFunction::Vec3InOutExponentInterpolationFunction() {}

std::string Vec3InOutExponentInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec3InOutExponentInterpolationFunction::getIdentifier() const {
    return "exponential";
}

tgt::vec3 Vec3InOutExponentInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutExponentInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutExponentInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutExponentInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InOutExponentInterpolationFunction::clone() const {
    return new Vec3InOutExponentInterpolationFunction();
}

Vec3InOutCircInterpolationFunction::Vec3InOutCircInterpolationFunction() {}

std::string Vec3InOutCircInterpolationFunction::getMode() const {
    return "first easing in, then easing out";
}

std::string Vec3InOutCircInterpolationFunction::getIdentifier() const {
    return "circular";
}

tgt::vec3 Vec3InOutCircInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::inOutCircInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::inOutCircInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::inOutCircInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3InOutCircInterpolationFunction::clone() const {
    return new Vec3InOutCircInterpolationFunction();
}

Vec3OutInQuadInterpolationFunction::Vec3OutInQuadInterpolationFunction() {}

std::string Vec3OutInQuadInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec3OutInQuadInterpolationFunction::getIdentifier() const {
    return "quadratic";
}

tgt::vec3 Vec3OutInQuadInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInQuadInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInQuadInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInQuadInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutInQuadInterpolationFunction::clone() const {
    return new Vec3OutInQuadInterpolationFunction();
}

Vec3OutInCubicInterpolationFunction::Vec3OutInCubicInterpolationFunction() {}

std::string Vec3OutInCubicInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec3OutInCubicInterpolationFunction::getIdentifier() const {
    return "cubic";
}

tgt::vec3 Vec3OutInCubicInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInCubicInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInCubicInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInCubicInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutInCubicInterpolationFunction::clone() const {
    return new Vec3OutInCubicInterpolationFunction();
}

Vec3OutInQuartInterpolationFunction::Vec3OutInQuartInterpolationFunction() {}

std::string Vec3OutInQuartInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec3OutInQuartInterpolationFunction::getIdentifier() const {
    return "quartetic";
}

tgt::vec3 Vec3OutInQuartInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInQuartInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInQuartInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInQuartInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutInQuartInterpolationFunction::clone() const {
    return new Vec3OutInQuartInterpolationFunction();
}

Vec3OutInQuintInterpolationFunction::Vec3OutInQuintInterpolationFunction() {}

std::string Vec3OutInQuintInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec3OutInQuintInterpolationFunction::getIdentifier() const {
    return "quintic";
}

tgt::vec3 Vec3OutInQuintInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInQuintInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInQuintInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInQuintInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutInQuintInterpolationFunction::clone() const {
    return new Vec3OutInQuintInterpolationFunction();
}

Vec3OutInSineInterpolationFunction::Vec3OutInSineInterpolationFunction() {}

std::string Vec3OutInSineInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec3OutInSineInterpolationFunction::getIdentifier() const {
    return "sineousidal";
}

tgt::vec3 Vec3OutInSineInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInSineInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInSineInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInSineInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutInSineInterpolationFunction::clone() const {
    return new Vec3OutInSineInterpolationFunction();
}

Vec3OutInExponentInterpolationFunction::Vec3OutInExponentInterpolationFunction() {}

std::string Vec3OutInExponentInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec3OutInExponentInterpolationFunction::getIdentifier() const {
    return "exponential";
}

tgt::vec3 Vec3OutInExponentInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInExponentInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInExponentInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInExponentInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutInExponentInterpolationFunction::clone() const {
    return new Vec3OutInExponentInterpolationFunction();
}

Vec3OutInCircInterpolationFunction::Vec3OutInCircInterpolationFunction() {}

std::string Vec3OutInCircInterpolationFunction::getMode() const {
    return "first easing out, then easing in";
}

std::string Vec3OutInCircInterpolationFunction::getIdentifier() const {
    return "circular";
}

tgt::vec3 Vec3OutInCircInterpolationFunction::interpolate(tgt::vec3 startvalue, tgt::vec3 endvalue, float time) const {
    float compX = BasicFloatInterpolation::outInCircInterpolation(startvalue.x, endvalue.x, time);
    float compY = BasicFloatInterpolation::outInCircInterpolation(startvalue.y, endvalue.y, time);
    float compZ = BasicFloatInterpolation::outInCircInterpolation(startvalue.z, endvalue.z, time);
    return tgt::vec3(compX, compY, compZ);
}

InterpolationFunction<tgt::vec3>* Vec3OutInCircInterpolationFunction::clone() const {
    return new Vec3OutInCircInterpolationFunction();
}

} // namespace voreen
