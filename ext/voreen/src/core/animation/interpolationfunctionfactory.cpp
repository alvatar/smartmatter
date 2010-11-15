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

#include "voreen/core/animation/interpolationfunctionfactory.h"

#include "voreen/core/animation/interpolation/floatinterpolationfunctions.h"
#include "voreen/core/animation/interpolation/intinterpolationfunctions.h"
#include "voreen/core/animation/interpolation/boolinterpolationfunctions.h"
#include "voreen/core/animation/interpolation/ivec2interpolationfunctions.h"
#include "voreen/core/animation/interpolation/ivec3interpolationfunctions.h"
#include "voreen/core/animation/interpolation/ivec4interpolationfunctions.h"
#include "voreen/core/animation/interpolation/vec2interpolationfunctions.h"
#include "voreen/core/animation/interpolation/vec3interpolationfunctions.h"
#include "voreen/core/animation/interpolation/vec4interpolationfunctions.h"
#include "voreen/core/animation/interpolation/mat2interpolationfunctions.h"
#include "voreen/core/animation/interpolation/mat3interpolationfunctions.h"
#include "voreen/core/animation/interpolation/mat4interpolationfunctions.h"
#include "voreen/core/animation/interpolation/camerainterpolationfunctions.h"
#include "voreen/core/animation/interpolation/shadersourceinterpolationfunctions.h"
#include "voreen/core/animation/interpolation/transfuncinterpolationfunctions.h"
#include "voreen/core/animation/interpolation/stringinterpolationfunctions.h"
#include "voreen/core/animation/interpolation/volumecollectioninterpolationfunctions.h"
#include "voreen/core/animation/interpolation/volumehandleinterpolationfunctions.h"

namespace voreen {

template <class T>
InterpolationFunctionFactory<T>::InterpolationFunctionFactory() {
    // Default functions
    registerFunction(new InterpolationFunction<int>());
    registerFunction(new InterpolationFunction<bool>());
    registerFunction(new InterpolationFunction<float>());
    registerFunction(new InterpolationFunction<tgt::ivec2>());
    registerFunction(new InterpolationFunction<tgt::ivec3>());
    registerFunction(new InterpolationFunction<tgt::ivec4>());
    registerFunction(new InterpolationFunction<tgt::vec2>());
    registerFunction(new InterpolationFunction<tgt::vec3>());
    registerFunction(new InterpolationFunction<tgt::vec4>());
    registerFunction(new InterpolationFunction<tgt::mat2>());
    registerFunction(new InterpolationFunction<tgt::mat3>());
    registerFunction(new InterpolationFunction<tgt::mat4>());
    registerFunction(new InterpolationFunction<tgt::Camera>());
    registerFunction(new InterpolationFunction<std::string>());
    registerFunction(new InterpolationFunction<ShaderSource>());
    registerFunction(new InterpolationFunction<TransFunc*>());
    registerFunction(new InterpolationFunction<VolumeCollection*>());
    registerFunction(new InterpolationFunction<VolumeHandle*>());

    //Float
    registerFunction(new FloatStartInterpolationFunction());
    registerFunction(new FloatEndInterpolationFunction());
    registerFunction(new FloatStartEndInterpolationFunction());
    registerFunction(new FloatLinearInterpolationFunction());
    registerFunction(new FloatCatmullRomInterpolationFunction());
    // Float: easing in
    registerFunction(new FloatInQuadInterpolationFunction());
    registerFunction(new FloatInCubicInterpolationFunction());
    registerFunction(new FloatInQuartInterpolationFunction());
    registerFunction(new FloatInQuintInterpolationFunction());
    registerFunction(new FloatInSineInterpolationFunction());
    registerFunction(new FloatInExponentInterpolationFunction());
    registerFunction(new FloatInCircInterpolationFunction());
    // Float: easing out
    registerFunction(new FloatOutQuadInterpolationFunction());
    registerFunction(new FloatOutCubicInterpolationFunction());
    registerFunction(new FloatOutQuartInterpolationFunction());
    registerFunction(new FloatOutQuintInterpolationFunction());
    registerFunction(new FloatOutSineInterpolationFunction());
    registerFunction(new FloatOutExponentInterpolationFunction());
    registerFunction(new FloatOutCircInterpolationFunction());
    // Float: easing in, easing out
    registerFunction(new FloatInOutQuadInterpolationFunction());
    registerFunction(new FloatInOutCubicInterpolationFunction());
    registerFunction(new FloatInOutQuartInterpolationFunction());
    registerFunction(new FloatInOutQuintInterpolationFunction());
    registerFunction(new FloatInOutSineInterpolationFunction());
    registerFunction(new FloatInOutExponentInterpolationFunction());
    registerFunction(new FloatInOutCircInterpolationFunction());
    // Float: easing out, easing in
    registerFunction(new FloatOutInQuadInterpolationFunction());
    registerFunction(new FloatOutInCubicInterpolationFunction());
    registerFunction(new FloatOutInQuartInterpolationFunction());
    registerFunction(new FloatOutInQuintInterpolationFunction());
    registerFunction(new FloatOutInSineInterpolationFunction());
    registerFunction(new FloatOutInExponentInterpolationFunction());
    registerFunction(new FloatOutInCircInterpolationFunction());

    //Int
    registerFunction(new IntStartInterpolationFunction());
    registerFunction(new IntEndInterpolationFunction());
    registerFunction(new IntStartEndInterpolationFunction());
    registerFunction(new IntLinearInterpolationFunction());
    registerFunction(new IntCatmullRomInterpolationFunction());
    // Int: easing in
    registerFunction(new IntInQuadInterpolationFunction());
    registerFunction(new IntInCubicInterpolationFunction());
    registerFunction(new IntInQuartInterpolationFunction());
    registerFunction(new IntInQuintInterpolationFunction());
    registerFunction(new IntInSineInterpolationFunction());
    registerFunction(new IntInExponentInterpolationFunction());
    registerFunction(new IntInCircInterpolationFunction());
    // Int: easing out
    registerFunction(new IntOutQuadInterpolationFunction());
    registerFunction(new IntOutCubicInterpolationFunction());
    registerFunction(new IntOutQuartInterpolationFunction());
    registerFunction(new IntOutQuintInterpolationFunction());
    registerFunction(new IntOutSineInterpolationFunction());
    registerFunction(new IntOutExponentInterpolationFunction());
    registerFunction(new IntOutCircInterpolationFunction());
    // Int: easing in, easing out
    registerFunction(new IntInOutQuadInterpolationFunction());
    registerFunction(new IntInOutCubicInterpolationFunction());
    registerFunction(new IntInOutQuartInterpolationFunction());
    registerFunction(new IntInOutQuintInterpolationFunction());
    registerFunction(new IntInOutSineInterpolationFunction());
    registerFunction(new IntInOutExponentInterpolationFunction());
    registerFunction(new IntInOutCircInterpolationFunction());
    // Int: easing out, easing in
    registerFunction(new IntOutInQuadInterpolationFunction());
    registerFunction(new IntOutInCubicInterpolationFunction());
    registerFunction(new IntOutInQuartInterpolationFunction());
    registerFunction(new IntOutInQuintInterpolationFunction());
    registerFunction(new IntOutInSineInterpolationFunction());
    registerFunction(new IntOutInExponentInterpolationFunction());
    registerFunction(new IntOutInCircInterpolationFunction());

    //Bool
    registerFunction(new BoolStartInterpolationFunction());
    registerFunction(new BoolEndInterpolationFunction());
    registerFunction(new BoolStartEndInterpolationFunction());
    //registerFunction(new BoolFlickerInterpolation()); ?

    // IVec2
    registerFunction(new IVec2StartInterpolationFunction());
    registerFunction(new IVec2EndInterpolationFunction());
    registerFunction(new IVec2StartEndInterpolationFunction());
    registerFunction(new IVec2LinearInterpolationFunction());
    // IVec2: easing in
    registerFunction(new IVec2InQuadInterpolationFunction());
    registerFunction(new IVec2InCubicInterpolationFunction());
    registerFunction(new IVec2InQuartInterpolationFunction());
    registerFunction(new IVec2InQuintInterpolationFunction());
    registerFunction(new IVec2InSineInterpolationFunction());
    registerFunction(new IVec2InExponentInterpolationFunction());
    registerFunction(new IVec2InCircInterpolationFunction());
    // IVec2: easing out
    registerFunction(new IVec2OutQuadInterpolationFunction());
    registerFunction(new IVec2OutCubicInterpolationFunction());
    registerFunction(new IVec2OutQuartInterpolationFunction());
    registerFunction(new IVec2OutQuintInterpolationFunction());
    registerFunction(new IVec2OutSineInterpolationFunction());
    registerFunction(new IVec2OutExponentInterpolationFunction());
    registerFunction(new IVec2OutCircInterpolationFunction());
    // IVec2: easing in, easing out
    registerFunction(new IVec2InOutQuadInterpolationFunction());
    registerFunction(new IVec2InOutCubicInterpolationFunction());
    registerFunction(new IVec2InOutQuartInterpolationFunction());
    registerFunction(new IVec2InOutQuintInterpolationFunction());
    registerFunction(new IVec2InOutSineInterpolationFunction());
    registerFunction(new IVec2InOutExponentInterpolationFunction());
    registerFunction(new IVec2InOutCircInterpolationFunction());
    // IVec2: easing out, easing in
    registerFunction(new IVec2OutInQuadInterpolationFunction());
    registerFunction(new IVec2OutInCubicInterpolationFunction());
    registerFunction(new IVec2OutInQuartInterpolationFunction());
    registerFunction(new IVec2OutInQuintInterpolationFunction());
    registerFunction(new IVec2OutInSineInterpolationFunction());
    registerFunction(new IVec2OutInExponentInterpolationFunction());
    registerFunction(new IVec2OutInCircInterpolationFunction());

    // IVec3
    registerFunction(new IVec3StartInterpolationFunction());
    registerFunction(new IVec3EndInterpolationFunction());
    registerFunction(new IVec3StartEndInterpolationFunction());
    registerFunction(new IVec3LinearInterpolationFunction());
    // IVec3: easing in
    registerFunction(new IVec3InQuadInterpolationFunction());
    registerFunction(new IVec3InCubicInterpolationFunction());
    registerFunction(new IVec3InQuartInterpolationFunction());
    registerFunction(new IVec3InQuintInterpolationFunction());
    registerFunction(new IVec3InSineInterpolationFunction());
    registerFunction(new IVec3InExponentInterpolationFunction());
    registerFunction(new IVec3InCircInterpolationFunction());
    // IVec3: easing out
    registerFunction(new IVec3OutQuadInterpolationFunction());
    registerFunction(new IVec3OutCubicInterpolationFunction());
    registerFunction(new IVec3OutQuartInterpolationFunction());
    registerFunction(new IVec3OutQuintInterpolationFunction());
    registerFunction(new IVec3OutSineInterpolationFunction());
    registerFunction(new IVec3OutExponentInterpolationFunction());
    registerFunction(new IVec3OutCircInterpolationFunction());
    // IVec3: easing in, easing out
    registerFunction(new IVec3InOutQuadInterpolationFunction());
    registerFunction(new IVec3InOutCubicInterpolationFunction());
    registerFunction(new IVec3InOutQuartInterpolationFunction());
    registerFunction(new IVec3InOutQuintInterpolationFunction());
    registerFunction(new IVec3InOutSineInterpolationFunction());
    registerFunction(new IVec3InOutExponentInterpolationFunction());
    registerFunction(new IVec3InOutCircInterpolationFunction());
    // IVec3: easing out, easing in
    registerFunction(new IVec3OutInQuadInterpolationFunction());
    registerFunction(new IVec3OutInCubicInterpolationFunction());
    registerFunction(new IVec3OutInQuartInterpolationFunction());
    registerFunction(new IVec3OutInQuintInterpolationFunction());
    registerFunction(new IVec3OutInSineInterpolationFunction());
    registerFunction(new IVec3OutInExponentInterpolationFunction());
    registerFunction(new IVec3OutInCircInterpolationFunction());

    // IVec4
    registerFunction(new IVec4StartInterpolationFunction());
    registerFunction(new IVec4EndInterpolationFunction());
    registerFunction(new IVec4StartEndInterpolationFunction());
    registerFunction(new IVec4LinearInterpolationFunction());
    // IVec4: easing in
    registerFunction(new IVec4InQuadInterpolationFunction());
    registerFunction(new IVec4InCubicInterpolationFunction());
    registerFunction(new IVec4InQuartInterpolationFunction());
    registerFunction(new IVec4InQuintInterpolationFunction());
    registerFunction(new IVec4InSineInterpolationFunction());
    registerFunction(new IVec4InExponentInterpolationFunction());
    registerFunction(new IVec4InCircInterpolationFunction());
    // IVec4: easing out
    registerFunction(new IVec4OutQuadInterpolationFunction());
    registerFunction(new IVec4OutCubicInterpolationFunction());
    registerFunction(new IVec4OutQuartInterpolationFunction());
    registerFunction(new IVec4OutQuintInterpolationFunction());
    registerFunction(new IVec4OutSineInterpolationFunction());
    registerFunction(new IVec4OutExponentInterpolationFunction());
    registerFunction(new IVec4OutCircInterpolationFunction());
    // IVec4: easing in, easing out
    registerFunction(new IVec4InOutQuadInterpolationFunction());
    registerFunction(new IVec4InOutCubicInterpolationFunction());
    registerFunction(new IVec4InOutQuartInterpolationFunction());
    registerFunction(new IVec4InOutQuintInterpolationFunction());
    registerFunction(new IVec4InOutSineInterpolationFunction());
    registerFunction(new IVec4InOutExponentInterpolationFunction());
    registerFunction(new IVec4InOutCircInterpolationFunction());
    // IVec4: easing out, easing in
    registerFunction(new IVec4OutInQuadInterpolationFunction());
    registerFunction(new IVec4OutInCubicInterpolationFunction());
    registerFunction(new IVec4OutInQuartInterpolationFunction());
    registerFunction(new IVec4OutInQuintInterpolationFunction());
    registerFunction(new IVec4OutInSineInterpolationFunction());
    registerFunction(new IVec4OutInExponentInterpolationFunction());
    registerFunction(new IVec4OutInCircInterpolationFunction());

    // Vec2
    registerFunction(new Vec2StartInterpolationFunction());
    registerFunction(new Vec2EndInterpolationFunction());
    registerFunction(new Vec2StartEndInterpolationFunction());
    registerFunction(new Vec2LinearInterpolationFunction());
    registerFunction(new Vec2SphericalLinearInterpolationFunction());
    // Vec2: easing in
    registerFunction(new Vec2InQuadInterpolationFunction());
    registerFunction(new Vec2InCubicInterpolationFunction());
    registerFunction(new Vec2InQuartInterpolationFunction());
    registerFunction(new Vec2InQuintInterpolationFunction());
    registerFunction(new Vec2InSineInterpolationFunction());
    registerFunction(new Vec2InExponentInterpolationFunction());
    registerFunction(new Vec2InCircInterpolationFunction());
    // Vec2: easing out
    registerFunction(new Vec2OutQuadInterpolationFunction());
    registerFunction(new Vec2OutCubicInterpolationFunction());
    registerFunction(new Vec2OutQuartInterpolationFunction());
    registerFunction(new Vec2OutQuintInterpolationFunction());
    registerFunction(new Vec2OutSineInterpolationFunction());
    registerFunction(new Vec2OutExponentInterpolationFunction());
    registerFunction(new Vec2OutCircInterpolationFunction());
    // Vec2: easing in, easing out
    registerFunction(new Vec2InOutQuadInterpolationFunction());
    registerFunction(new Vec2InOutCubicInterpolationFunction());
    registerFunction(new Vec2InOutQuartInterpolationFunction());
    registerFunction(new Vec2InOutQuintInterpolationFunction());
    registerFunction(new Vec2InOutSineInterpolationFunction());
    registerFunction(new Vec2InOutExponentInterpolationFunction());
    registerFunction(new Vec2InOutCircInterpolationFunction());
    // Vec2: easing out, easing in
    registerFunction(new Vec2OutInQuadInterpolationFunction());
    registerFunction(new Vec2OutInCubicInterpolationFunction());
    registerFunction(new Vec2OutInQuartInterpolationFunction());
    registerFunction(new Vec2OutInQuintInterpolationFunction());
    registerFunction(new Vec2OutInSineInterpolationFunction());
    registerFunction(new Vec2OutInExponentInterpolationFunction());
    registerFunction(new Vec2OutInCircInterpolationFunction());

    // Vec3
    registerFunction(new Vec3StartInterpolationFunction());
    registerFunction(new Vec3EndInterpolationFunction());
    registerFunction(new Vec3StartEndInterpolationFunction());
    registerFunction(new Vec3LinearInterpolationFunction());
    registerFunction(new Vec3SphericalLinearInterpolationFunction());
    registerFunction(new Vec3SphericalCubicInterpolationFunction());
    // Vec3: easing in
    registerFunction(new Vec3InQuadInterpolationFunction());
    registerFunction(new Vec3InCubicInterpolationFunction());
    registerFunction(new Vec3InQuartInterpolationFunction());
    registerFunction(new Vec3InQuintInterpolationFunction());
    registerFunction(new Vec3InSineInterpolationFunction());
    registerFunction(new Vec3InExponentInterpolationFunction());
    registerFunction(new Vec3InCircInterpolationFunction());
    // Vec3: easing out
    registerFunction(new Vec3OutQuadInterpolationFunction());
    registerFunction(new Vec3OutCubicInterpolationFunction());
    registerFunction(new Vec3OutQuartInterpolationFunction());
    registerFunction(new Vec3OutQuintInterpolationFunction());
    registerFunction(new Vec3OutSineInterpolationFunction());
    registerFunction(new Vec3OutExponentInterpolationFunction());
    registerFunction(new Vec3OutCircInterpolationFunction());
    // Vec3: easing in, easing out
    registerFunction(new Vec3InOutQuadInterpolationFunction());
    registerFunction(new Vec3InOutCubicInterpolationFunction());
    registerFunction(new Vec3InOutQuartInterpolationFunction());
    registerFunction(new Vec3InOutQuintInterpolationFunction());
    registerFunction(new Vec3InOutSineInterpolationFunction());
    registerFunction(new Vec3InOutExponentInterpolationFunction());
    registerFunction(new Vec3InOutCircInterpolationFunction());
    // Vec3: easing out, easing in
    registerFunction(new Vec3OutInQuadInterpolationFunction());
    registerFunction(new Vec3OutInCubicInterpolationFunction());
    registerFunction(new Vec3OutInQuartInterpolationFunction());
    registerFunction(new Vec3OutInQuintInterpolationFunction());
    registerFunction(new Vec3OutInSineInterpolationFunction());
    registerFunction(new Vec3OutInExponentInterpolationFunction());
    registerFunction(new Vec3OutInCircInterpolationFunction());

    // Vec4
    registerFunction(new Vec4StartInterpolationFunction());
    registerFunction(new Vec4EndInterpolationFunction());
    registerFunction(new Vec4StartEndInterpolationFunction());
    registerFunction(new Vec4LinearInterpolationFunction());
    // Vec4: easing in
    registerFunction(new Vec4InQuadInterpolationFunction());
    registerFunction(new Vec4InCubicInterpolationFunction());
    registerFunction(new Vec4InQuartInterpolationFunction());
    registerFunction(new Vec4InQuintInterpolationFunction());
    registerFunction(new Vec4InSineInterpolationFunction());
    registerFunction(new Vec4InExponentInterpolationFunction());
    registerFunction(new Vec4InCircInterpolationFunction());
    // Vec4: easing out
    registerFunction(new Vec4OutQuadInterpolationFunction());
    registerFunction(new Vec4OutCubicInterpolationFunction());
    registerFunction(new Vec4OutQuartInterpolationFunction());
    registerFunction(new Vec4OutQuintInterpolationFunction());
    registerFunction(new Vec4OutSineInterpolationFunction());
    registerFunction(new Vec4OutExponentInterpolationFunction());
    registerFunction(new Vec4OutCircInterpolationFunction());
    // Vec4: easing in, easing out
    registerFunction(new Vec4InOutQuadInterpolationFunction());
    registerFunction(new Vec4InOutCubicInterpolationFunction());
    registerFunction(new Vec4InOutQuartInterpolationFunction());
    registerFunction(new Vec4InOutQuintInterpolationFunction());
    registerFunction(new Vec4InOutSineInterpolationFunction());
    registerFunction(new Vec4InOutExponentInterpolationFunction());
    registerFunction(new Vec4InOutCircInterpolationFunction());
    // Vec4: easing out, easing in
    registerFunction(new Vec4OutInQuadInterpolationFunction());
    registerFunction(new Vec4OutInCubicInterpolationFunction());
    registerFunction(new Vec4OutInQuartInterpolationFunction());
    registerFunction(new Vec4OutInQuintInterpolationFunction());
    registerFunction(new Vec4OutInSineInterpolationFunction());
    registerFunction(new Vec4OutInExponentInterpolationFunction());
    registerFunction(new Vec4OutInCircInterpolationFunction());

    // Mat2
    registerFunction(new Mat2StartInterpolationFunction());
    registerFunction(new Mat2EndInterpolationFunction());
    registerFunction(new Mat2StartEndInterpolationFunction());
    registerFunction(new Mat2LinearInterpolationFunction());
    // Mat2: easing in
    registerFunction(new Mat2InQuadInterpolationFunction());
    registerFunction(new Mat2InCubicInterpolationFunction());
    registerFunction(new Mat2InQuartInterpolationFunction());
    registerFunction(new Mat2InQuintInterpolationFunction());
    registerFunction(new Mat2InSineInterpolationFunction());
    registerFunction(new Mat2InExponentInterpolationFunction());
    registerFunction(new Mat2InCircInterpolationFunction());
    // Mat2: easing out
    registerFunction(new Mat2OutQuadInterpolationFunction());
    registerFunction(new Mat2OutCubicInterpolationFunction());
    registerFunction(new Mat2OutQuartInterpolationFunction());
    registerFunction(new Mat2OutQuintInterpolationFunction());
    registerFunction(new Mat2OutSineInterpolationFunction());
    registerFunction(new Mat2OutExponentInterpolationFunction());
    registerFunction(new Mat2OutCircInterpolationFunction());
    // Mat2: easing in, easing out
    registerFunction(new Mat2InOutQuadInterpolationFunction());
    registerFunction(new Mat2InOutCubicInterpolationFunction());
    registerFunction(new Mat2InOutQuartInterpolationFunction());
    registerFunction(new Mat2InOutQuintInterpolationFunction());
    registerFunction(new Mat2InOutSineInterpolationFunction());
    registerFunction(new Mat2InOutExponentInterpolationFunction());
    registerFunction(new Mat2InOutCircInterpolationFunction());
    // Mat2: easing out, easing in
    registerFunction(new Mat2OutInQuadInterpolationFunction());
    registerFunction(new Mat2OutInCubicInterpolationFunction());
    registerFunction(new Mat2OutInQuartInterpolationFunction());
    registerFunction(new Mat2OutInQuintInterpolationFunction());
    registerFunction(new Mat2OutInSineInterpolationFunction());
    registerFunction(new Mat2OutInExponentInterpolationFunction());
    registerFunction(new Mat2OutInCircInterpolationFunction());

    // Mat3
    registerFunction(new Mat3StartInterpolationFunction());
    registerFunction(new Mat3EndInterpolationFunction());
    registerFunction(new Mat3StartEndInterpolationFunction());
    registerFunction(new Mat3LinearInterpolationFunction());
    // Mat3: easing in
    registerFunction(new Mat3InQuadInterpolationFunction());
    registerFunction(new Mat3InCubicInterpolationFunction());
    registerFunction(new Mat3InQuartInterpolationFunction());
    registerFunction(new Mat3InQuintInterpolationFunction());
    registerFunction(new Mat3InSineInterpolationFunction());
    registerFunction(new Mat3InExponentInterpolationFunction());
    registerFunction(new Mat3InCircInterpolationFunction());
    // Mat3: easing out
    registerFunction(new Mat3OutQuadInterpolationFunction());
    registerFunction(new Mat3OutCubicInterpolationFunction());
    registerFunction(new Mat3OutQuartInterpolationFunction());
    registerFunction(new Mat3OutQuintInterpolationFunction());
    registerFunction(new Mat3OutSineInterpolationFunction());
    registerFunction(new Mat3OutExponentInterpolationFunction());
    registerFunction(new Mat3OutCircInterpolationFunction());
    // Mat3: easing in, easing out
    registerFunction(new Mat3InOutQuadInterpolationFunction());
    registerFunction(new Mat3InOutCubicInterpolationFunction());
    registerFunction(new Mat3InOutQuartInterpolationFunction());
    registerFunction(new Mat3InOutQuintInterpolationFunction());
    registerFunction(new Mat3InOutSineInterpolationFunction());
    registerFunction(new Mat3InOutExponentInterpolationFunction());
    registerFunction(new Mat3InOutCircInterpolationFunction());
    // Mat3: easing out, easing in
    registerFunction(new Mat3OutInQuadInterpolationFunction());
    registerFunction(new Mat3OutInCubicInterpolationFunction());
    registerFunction(new Mat3OutInQuartInterpolationFunction());
    registerFunction(new Mat3OutInQuintInterpolationFunction());
    registerFunction(new Mat3OutInSineInterpolationFunction());
    registerFunction(new Mat3OutInExponentInterpolationFunction());
    registerFunction(new Mat3OutInCircInterpolationFunction());

    // Mat4
    registerFunction(new Mat4StartInterpolationFunction());
    registerFunction(new Mat4EndInterpolationFunction());
    registerFunction(new Mat4StartEndInterpolationFunction());
    registerFunction(new Mat4LinearInterpolationFunction());
    // Mat4: easing in
    registerFunction(new Mat4InQuadInterpolationFunction());
    registerFunction(new Mat4InCubicInterpolationFunction());
    registerFunction(new Mat4InQuartInterpolationFunction());
    registerFunction(new Mat4InQuintInterpolationFunction());
    registerFunction(new Mat4InSineInterpolationFunction());
    registerFunction(new Mat4InExponentInterpolationFunction());
    registerFunction(new Mat4InCircInterpolationFunction());
    // Mat4: easing out
    registerFunction(new Mat4OutQuadInterpolationFunction());
    registerFunction(new Mat4OutCubicInterpolationFunction());
    registerFunction(new Mat4OutQuartInterpolationFunction());
    registerFunction(new Mat4OutQuintInterpolationFunction());
    registerFunction(new Mat4OutSineInterpolationFunction());
    registerFunction(new Mat4OutExponentInterpolationFunction());
    registerFunction(new Mat4OutCircInterpolationFunction());
    // Mat4: easing in, easing out
    registerFunction(new Mat4InOutQuadInterpolationFunction());
    registerFunction(new Mat4InOutCubicInterpolationFunction());
    registerFunction(new Mat4InOutQuartInterpolationFunction());
    registerFunction(new Mat4InOutQuintInterpolationFunction());
    registerFunction(new Mat4InOutSineInterpolationFunction());
    registerFunction(new Mat4InOutExponentInterpolationFunction());
    registerFunction(new Mat4InOutCircInterpolationFunction());
    // Mat4: easing out, easing in
    registerFunction(new Mat4OutInQuadInterpolationFunction());
    registerFunction(new Mat4OutInCubicInterpolationFunction());
    registerFunction(new Mat4OutInQuartInterpolationFunction());
    registerFunction(new Mat4OutInQuintInterpolationFunction());
    registerFunction(new Mat4OutInSineInterpolationFunction());
    registerFunction(new Mat4OutInExponentInterpolationFunction());
    registerFunction(new Mat4OutInCircInterpolationFunction());

    //Camera
    registerFunction(new CameraStartInterpolationFunction());
    registerFunction(new CameraEndInterpolationFunction());
    registerFunction(new CameraStartEndInterpolationFunction());
    registerFunction(new CameraLinearInterpolationFunction());
    registerFunction(new CameraSphericalLinearInterpolationFunction());
    registerFunction(new CameraCubicSplineInterpolationFunction());
    registerFunction(new CameraCatmullRomInterpolationFunction());
    registerFunction(new CameraSquadInterpolationFunction());

    //ShaderSource
    registerFunction(new ShaderSourceStartInterpolationFunction());
    registerFunction(new ShaderSourceEndInterpolationFunction());
    registerFunction(new ShaderSourceStartEndInterpolationFunction());

    //String
    registerFunction(new StringStartInterpolationFunction());
    registerFunction(new StringEndInterpolationFunction());
    registerFunction(new StringStartEndInterpolationFunction());

    //TransFunc
    registerFunction(new TransFuncStartInterpolationFunction());
    registerFunction(new TransFuncEndInterpolationFunction());
    registerFunction(new TransFuncStartEndInterpolationFunction());
    // keywise
    registerFunction(new TransFuncKeyWiseInterpolationFunction());
    registerFunction(new TransFuncKeyWiseQuadInInterpolationFunction());
    registerFunction(new TransFuncKeyWiseQuadOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseQuadInOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseQuadOutInInterpolationFunction());
    registerFunction(new TransFuncKeyWiseCubicInInterpolationFunction());
    registerFunction(new TransFuncKeyWiseCubicOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseCubicInOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseCubicOutInInterpolationFunction());
    registerFunction(new TransFuncKeyWiseQuartInInterpolationFunction());
    registerFunction(new TransFuncKeyWiseQuartOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseQuartInOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseQuartOutInInterpolationFunction());
    registerFunction(new TransFuncKeyWiseQuintInInterpolationFunction());
    registerFunction(new TransFuncKeyWiseQuintOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseQuintInOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseQuintOutInInterpolationFunction());
    registerFunction(new TransFuncKeyWiseSineInInterpolationFunction());
    registerFunction(new TransFuncKeyWiseSineOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseSineInOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseSineOutInInterpolationFunction());
    registerFunction(new TransFuncKeyWiseExponentInInterpolationFunction());
    registerFunction(new TransFuncKeyWiseExponentOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseExponentInOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseExponentOutInInterpolationFunction());
    registerFunction(new TransFuncKeyWiseCircInInterpolationFunction());
    registerFunction(new TransFuncKeyWiseCircOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseCircInOutInterpolationFunction());
    registerFunction(new TransFuncKeyWiseCircOutInInterpolationFunction());
    // texture
    registerFunction(new TransFuncTextureLinearInterpolationFunction());
    registerFunction(new TransFuncTextureQuadInInterpolationFunction());
    registerFunction(new TransFuncTextureQuadOutInterpolationFunction());
    registerFunction(new TransFuncTextureQuadInOutInterpolationFunction());
    registerFunction(new TransFuncTextureQuadOutInInterpolationFunction());
    registerFunction(new TransFuncTextureCubicInInterpolationFunction());
    registerFunction(new TransFuncTextureCubicOutInterpolationFunction());
    registerFunction(new TransFuncTextureCubicInOutInterpolationFunction());
    registerFunction(new TransFuncTextureCubicOutInInterpolationFunction());
    registerFunction(new TransFuncTextureQuartInInterpolationFunction());
    registerFunction(new TransFuncTextureQuartOutInterpolationFunction());
    registerFunction(new TransFuncTextureQuartInOutInterpolationFunction());
    registerFunction(new TransFuncTextureQuartOutInInterpolationFunction());
    registerFunction(new TransFuncTextureQuintInInterpolationFunction());
    registerFunction(new TransFuncTextureQuintOutInterpolationFunction());
    registerFunction(new TransFuncTextureQuintInOutInterpolationFunction());
    registerFunction(new TransFuncTextureQuintOutInInterpolationFunction());
    registerFunction(new TransFuncTextureSineInInterpolationFunction());
    registerFunction(new TransFuncTextureSineOutInterpolationFunction());
    registerFunction(new TransFuncTextureSineInOutInterpolationFunction());
    registerFunction(new TransFuncTextureSineOutInInterpolationFunction());
    registerFunction(new TransFuncTextureExponentInInterpolationFunction());
    registerFunction(new TransFuncTextureExponentOutInterpolationFunction());
    registerFunction(new TransFuncTextureExponentInOutInterpolationFunction());
    registerFunction(new TransFuncTextureExponentOutInInterpolationFunction());
    registerFunction(new TransFuncTextureCircInInterpolationFunction());
    registerFunction(new TransFuncTextureCircOutInterpolationFunction());
    registerFunction(new TransFuncTextureCircInOutInterpolationFunction());
    registerFunction(new TransFuncTextureCircOutInInterpolationFunction());

    //VolumeCollection
    registerFunction(new VolumeCollectionStartInterpolationFunction());
    registerFunction(new VolumeCollectionEndInterpolationFunction());
    registerFunction(new VolumeCollectionStartEndInterpolationFunction());

    //VolumeHandle
    registerFunction(new VolumeHandleStartInterpolationFunction());
    registerFunction(new VolumeHandleEndInterpolationFunction());
    registerFunction(new VolumeHandleStartEndInterpolationFunction());
}


template <class T>
InterpolationFunctionFactory<T>::~InterpolationFunctionFactory() {
    typename std::vector<InterpolationFunction<T>*>::const_iterator it;
    for (it = functions_.begin(); it != functions_.end(); ++it) {
        delete (*it);
    }
    functions_.clear();
}

template <class T>
const std::vector<std::string> InterpolationFunctionFactory<T>::getListOfFunctionNames() const {
    std::vector<std::string> names;
    typename std::vector<InterpolationFunction<T>*>::const_iterator it;
    for (it = functions_.begin(); it != functions_.end(); ++it)
        names.push_back((*it)->getName());

    return names;
}

template <class T>
const std::vector<InterpolationFunction<T>*>& InterpolationFunctionFactory<T>::getListOfFunctions() const {
    return functions_;
}

template <class T>
InterpolationFunction<T>* InterpolationFunctionFactory<T>::getFunctionByName(const std::string& name) const {
    typename std::vector<InterpolationFunction<T>*>::const_iterator it;
    for (it = functions_.begin(); it != functions_.end(); ++it) {
        if (name.compare((*it)->getName()) == 0)
            return (*it)->clone();
    }

    return new InterpolationFunction<T>();
}

template <class T>
void InterpolationFunctionFactory<T>::registerFunction(InterpolationFunctionBase* func) {
    InterpolationFunction<T>* f;
    f = dynamic_cast<InterpolationFunction<T>*>(func);
    if (f)
        functions_.push_back(f);
    else
        delete func;
}

/**
* @see SerializableFactory::getTypeString
*/
template <class T>
const std::string InterpolationFunctionFactory<T>::getTypeString(const std::type_info& type) const {
    typename std::vector<InterpolationFunction<T>*>::const_iterator it;
    for (it = functions_.begin(); it != functions_.end(); ++it) {
        if (type == typeid(**it))
            return (*it)->getName();
    }

    return "";
}

/**
* @see SerializableFactory::createType
*/
template <class T>
Serializable* InterpolationFunctionFactory<T>::createType(const std::string& typeString) {
    typename std::vector<InterpolationFunction<T>*>::const_iterator it;

    for (it = functions_.begin(); it != functions_.end(); ++it) {
        if (typeString == (*it)->getName())
            return (*it)->clone();
    }

    return 0;
}

template <class T>
InterpolationFunctionFactory<T>* InterpolationFunctionFactory<T>::instance_ = 0;

template <class T>
InterpolationFunctionFactory<T>* InterpolationFunctionFactory<T>::getInstance() {
    if (!instance_)
        instance_ = new InterpolationFunctionFactory<T>();

    return instance_;
}

template class InterpolationFunctionFactory<float>;
template class InterpolationFunctionFactory<int>;
template class InterpolationFunctionFactory<bool>;
template class InterpolationFunctionFactory<tgt::ivec2>;
template class InterpolationFunctionFactory<tgt::ivec3>;
template class InterpolationFunctionFactory<tgt::ivec4>;
template class InterpolationFunctionFactory<tgt::vec2>;
template class InterpolationFunctionFactory<tgt::vec3>;
template class InterpolationFunctionFactory<tgt::vec4>;
template class InterpolationFunctionFactory<tgt::mat2>;
template class InterpolationFunctionFactory<tgt::mat3>;
template class InterpolationFunctionFactory<tgt::mat4>;
template class InterpolationFunctionFactory<tgt::Camera>;
template class InterpolationFunctionFactory<std::string>;
template class InterpolationFunctionFactory<ShaderSource>;
template class InterpolationFunctionFactory<TransFunc*>;
template class InterpolationFunctionFactory<VolumeCollection*>;
template class InterpolationFunctionFactory<VolumeHandle*>;

} // namespace voreen
