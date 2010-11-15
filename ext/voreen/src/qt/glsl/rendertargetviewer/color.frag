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

#version 130

#include "modules/mod_sampler2d.frag"

out vec4 FragData0;

uniform SAMPLER2D_TYPE tex_;
uniform TEXTURE_PARAMETERS texParameters_;

uniform bool enableColorR_;
uniform bool enableColorG_;
uniform bool enableColorB_;
uniform bool enableColorA_;
uniform bool showAlpha_;
uniform bool showDepth_;
uniform bool showHue_;
uniform bool showSaturation_;
uniform bool showValue_;
uniform float minDepth_;
uniform float maxDepth_;
uniform bool enableBackgroundCheckerboardPattern_;
uniform int checkerboardPatternDimension_;
uniform float backgroundColorR_;
uniform float backgroundColorG_;
uniform float backgroundColorB_;

vec4 rgbToHsv(vec4 rgb) {
    vec4 hsv;
    float min = min(rgb.r, min(rgb.g, rgb.b));
    float max = max(rgb.r, max(rgb.g, rgb.b));
    float diff = max - min;
    if (max == min)
        hsv.x = 0.0;
    else if (max == rgb.r)
        hsv.x = 60.0 * (0.0 + rgb.g - rgb.b / diff);
    else if (max == rgb.g)
        hsv.x = 60.0 * (2.0 + rgb.b - rgb.r / diff);
    else if (max == rgb.b)
        hsv.x = 60.0 * (4.0 + rgb.r - rgb.g / diff);
    if (hsv.x < 0.0)
        hsv.x += 360.0;
    if (max == 0.0)
        hsv.y = 0.0;
    else
        hsv.y = diff / max;
    hsv.z = max;
    hsv.w = rgb.a;
    return hsv;
}

void main() {
    vec2 coord = gl_TexCoord[0].xy;
    vec4 color = textureLookup2Dnormalized(tex_, texParameters_, coord);
    vec4 hsv = rgbToHsv(color);

    if (showValue_) {
        color = vec4(hsv.z, hsv.z, hsv.z, hsv.a);
    } else if (showSaturation_) {
        color = vec4(hsv.y, hsv.y, hsv.y, hsv.a);
    } else if (showHue_) {
        color = vec4(hsv.x / 360.0, hsv.x / 360.0, hsv.x / 360.0, hsv.a);
    } else if (showDepth_) {
        float depth = (color.r - minDepth_) / (maxDepth_ - minDepth_);;
        color = vec4(depth, depth, depth, 1.0);
    } else if (showAlpha_) {
        color = vec4(color.a, color.a, color.a, 1.0);
    } else {
        color.r = enableColorR_ ? color.r : 0.0;
        color.g = enableColorG_ ? color.g : 0.0;
        color.b = enableColorB_ ? color.b : 0.0;
        color.a = enableColorA_ ? color.a : 1.0;
    }

    if (enableBackgroundCheckerboardPattern_) {
        vec4 color1 = vec4(0.75, 0.75, 0.75, 1.0);
        vec4 color2 = vec4(0.50, 0.50, 0.50, 1.0);
        float tmp = (coord.x * float(checkerboardPatternDimension_)) + (coord.y * float(checkerboardPatternDimension_));
        if (int(tmp/2.0) - int(floor(tmp/2.0)) == 0)
            color = mix(color1, color, color.a);
        else
            color = mix(color2, color, color.a);
    } else {
        color = mix(vec4(backgroundColorR_, backgroundColorG_, backgroundColorB_, 1.0), color, color.a);
    }

    FragData0 = color;
}
