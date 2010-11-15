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

#include "modules/mod_sampler2d.frag"

uniform SAMPLER2D_TYPE colorTex0_;
uniform SAMPLER2D_TYPE depthTex0_;
uniform TEXTURE_PARAMETERS textureParameters0_;
uniform SAMPLER2D_TYPE colorTex1_;
uniform SAMPLER2D_TYPE depthTex1_;
uniform TEXTURE_PARAMETERS textureParameters1_;

uniform int compositingMode_;
uniform float weightingFactor_;

void main() {
    vec2 fragCoord = gl_FragCoord.xy;

    // fetch input textures
    vec4 color0 = textureLookup2Dscreen(colorTex0_, textureParameters0_, fragCoord);
    float depth0 = textureLookup2Dscreen(depthTex0_, textureParameters0_, fragCoord).z;
    vec4 color1 = textureLookup2Dscreen(colorTex1_, textureParameters1_, fragCoord);
    float depth1 = textureLookup2Dscreen(depthTex1_, textureParameters1_, fragCoord).z;

    vec4 fragColor = vec4(0.0);
    float fragDepth = 1.0;

    #if defined(MODE_DEPTH_TEST)
        if (depth0 < depth1) {
            fragColor = color0;
            fragDepth = depth0;
        }
        else {
            fragColor = color1;
            fragDepth = depth1;
        }
    #elif defined(MODE_ALPHA_COMPOSITING)
        if (depth1 < depth0) {
            fragColor.rgb = color1.rgb + color0.rgb * color0.a * (1.0 - color1.a);
            fragColor.a = color1.a + color0.a * (1.0 - color1.a);
        }
        else {
            fragColor.rgb = color0.rgb + color1.rgb * color1.a * (1.0 - color0.a);
            fragColor.a = color0.a + color1.a * (1.0 - color0.a);
        }
        fragDepth = min(depth0, depth1);
    #elif defined(MODE_ALPHA_BLENDING)
        fragColor.rgb = color0.a * color0.rgb + (1.0 - color0.a) * color1.rgb;
        fragColor.a = color0.a + color1.a * (1.0 - color0.a);
        if (color0.a > 0)
            fragDepth = depth0;
        else
            fragDepth = depth1;
    #elif defined(MODE_ALPHA_BLENDING_B_OVER_A)
        fragColor.rgb = color1.a * color1.rgb + (1.0 - color1.a) * color0.rgb;
        fragColor.a = color1.a + color0.a * (1.0 - color1.a);
        if (color1.a > 0)
            fragDepth = depth1;
        else
            fragDepth = depth0;
    #elif defined(MODE_WEIGHTED_AVERAGE)
        if (weightingFactor_ == 0.0)
            fragDepth = depth1;
        else if (weightingFactor_ == 1.0)
            fragDepth = depth0;
        else
            fragDepth = min(depth0, depth1);
        fragColor = weightingFactor_*color0 + (1.0-weightingFactor_)*color1;
    #elif defined(MODE_TAKE_FIRST)
        fragColor = color0;
        fragDepth = depth0;
    #elif defined(MODE_TAKE_SECOND)
        fragColor = color1;
        fragDepth = depth1;
    #elif defined(MODE_FIRST_HAS_PRIORITY)
        if (color0.a > 0.0) {
            fragColor = color0;
            fragDepth = depth0;
        }
        else {
            fragColor = color1;
            fragDepth = depth1;
        }
    #elif defined(MODE_SECOND_HAS_PRIORITY)
        // second has priority
        if (color1.a > 0.0) {
            fragColor = color1;
            fragDepth = depth1;
        }
        else {
            fragColor = color0;
            fragDepth = depth0;
        }
    #elif defined(MODE_MAXIMUM_ALPHA)
        fragDepth = min(depth0, depth1);
        fragColor.rgb = color0.rgb*color0.a + color1.rgb*color1.a;
        fragColor.a = max(color0.a, color1.a);
    #elif defined(MODE_DIFFERENCE)
        fragDepth = abs(depth0 - depth1);
        float diffR = 1 - abs(color0.r - color1.r);
        float diffG = 1 - abs(color0.g - color1.g);
        float diffB = 1 - abs(color0.b - color1.b);
        fragColor.rgba = vec4(diffR, diffG, diffB, max(color0.a,color1.a));
    #endif

    FragData0 = fragColor;
    gl_FragDepth = fragDepth;
}
