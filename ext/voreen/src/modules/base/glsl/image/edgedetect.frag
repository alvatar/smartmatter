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
#include "modules/mod_normdepth.frag"

uniform SAMPLER2D_TYPE colorTex_;
uniform SAMPLER2D_TYPE depthTex_;
uniform TEXTURE_PARAMETERS texParams_;

uniform vec4 edgeColor_;
uniform vec4 backgroundColor_;
uniform float edgeThreshold_;
uniform bool showImage_;
uniform int blendMode_;
uniform int edgeStyle_;


/***
 * Performs an image based edge detection based on the depth value.
 * To determine the edges, a Sobel filter is applied.
 *
 * @fragCoord - screen coordinates of the current fragment
 * @delta     - specifies the distance to the neighboor texels to be fetched,
 *              1.0 defines a one pixel distance
 ***/
vec2 edgeDetectionDepth(in vec2 fragCoord, in float delta) {
    float N = normDepth(textureLookup2Dscreen(depthTex_, texParams_, fragCoord+vec2(0.0,-delta)).z);
    float NE = normDepth(textureLookup2Dscreen(depthTex_, texParams_, fragCoord+vec2(delta,-delta)).z);
    float E = normDepth(textureLookup2Dscreen(depthTex_, texParams_, fragCoord+vec2(delta, 0.0)).z);
    float SE = normDepth(textureLookup2Dscreen(depthTex_, texParams_, fragCoord+vec2(delta, delta)).z);
    float S = normDepth(textureLookup2Dscreen(depthTex_, texParams_, fragCoord+vec2(0.0, delta)).z);
    float SW = normDepth(textureLookup2Dscreen(depthTex_, texParams_, fragCoord+vec2(-delta, delta)).z);
    float W = normDepth(textureLookup2Dscreen(depthTex_, texParams_, fragCoord+vec2(-delta, 0.0)).z);
    float NW = normDepth(textureLookup2Dscreen(depthTex_, texParams_, fragCoord+vec2(-delta,-delta)).z);

    vec2 gradient;
    gradient.x = +1.0*NW + 2.0*W + 1.0*SW - 1.0*NE - 2.0*E - 1.0*SE;
    gradient.y = +1.0*NW + 2.0*N + 1.0*NE - 1.0*SW - 2.0*S - 1.0*SE;
    return gradient;
}

/***
 * Draws a one pixel thick halo around the objects.
 *
 * @fragCoord - screen coordinates of the current fragment
 ***/
vec2 silhouetteDetectionColor(in vec2 fragCoord) {
    vec2 result = vec2(0.0);
    vec3 C = textureLookup2Dscreen(colorTex_, texParams_, fragCoord).rgb;
    if (C != backgroundColor_.rgb) {
        vec3 E = textureLookup2Dscreen(colorTex_, texParams_, fragCoord+vec2(1.0,0.0)).rgb;
        vec3 S = textureLookup2Dscreen(colorTex_, texParams_, fragCoord+vec2(0.0,-1.0)).rgb;
        vec3 W = textureLookup2Dscreen(colorTex_, texParams_, fragCoord+vec2(-1.0,0.0)).rgb;
        vec3 N = textureLookup2Dscreen(colorTex_, texParams_, fragCoord+vec2(0.0,1.0)).rgb;

        if (E == backgroundColor_.rgb || S == backgroundColor_.rgb ||
            W == backgroundColor_.rgb || N == backgroundColor_.rgb) {
            result = vec2(1.0);
        } else {
            vec3 SE = textureLookup2Dscreen(colorTex_, texParams_, fragCoord+vec2( 1.0,-1.0)).rgb;
            vec3 SW = textureLookup2Dscreen(colorTex_, texParams_, fragCoord+vec2(-1.0,-1.0)).rgb;
            vec3 NW = textureLookup2Dscreen(colorTex_, texParams_, fragCoord+vec2(-1.0, 1.0)).rgb;
            vec3 NE = textureLookup2Dscreen(colorTex_, texParams_, fragCoord+vec2( 1.0, 1.0)).rgb;
            if (SE == backgroundColor_.rgb || SW == backgroundColor_.rgb ||
                NW == backgroundColor_.rgb || NE == backgroundColor_.rgb) {
                result = vec2(1.0);
            }
        }
    }
    return result;
}


/***
 * The main method.
 ***/
void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    vec4 color = textureLookup2Dscreen(colorTex_, texParams_, fragCoord);

    // read and normalize depth value
    float depth = textureLookup2Dscreen(depthTex_, texParams_, fragCoord).z;
    float depthNorm = normDepth(depth);

    vec4 result;
    vec2 gradient;
    if (edgeStyle_ == 0) {
        // contour edges
        gradient = edgeDetectionDepth(fragCoord, 1.0);
    } else if (edgeStyle_ == 1) {
        // silhouette edges
        gradient = silhouetteDetectionColor(fragCoord);
    } else if (edgeStyle_ == 2) {
        // contour edges where edge thickness decreases with increasing depth
        gradient = edgeDetectionDepth(fragCoord, (1.0-depthNorm)*2.0);
    }

    float edgeThickness = length(gradient);
    if (edgeThickness > edgeThreshold_) {
        if (blendMode_ == 0) {
            // use edge color
            result = edgeColor_;
        } else if (blendMode_ == 1) {
            // use pseudo chromadepth
            result = vec4(1.0-depthNorm, 0.0, depthNorm, 1.0)*2.0;
        } else if (blendMode_ == 2) {
            // blend edges
            result = 0.5*edgeColor_ + 0.5*vec4(color.rgb, 1.0);
        } else if (blendMode_ == 3) {
            // output gradient
            result = vec4(gradient, 0.0, 1.0);
        }
    } else {
        if (showImage_)
            result = color;
        else {
            result = backgroundColor_;
            depth = 1.0;
        }
    }

    FragData0 = result;
    gl_FragDepth = depth;
}
