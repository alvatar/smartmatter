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
uniform TEXTURE_PARAMETERS texParams0_;
uniform SAMPLER2D_TYPE colorTex1_;
uniform SAMPLER2D_TYPE depthTex1_;
uniform TEXTURE_PARAMETERS texParams1_;

void main() {
    vec2 p = gl_FragCoord.xy * screenDimRCP_;
    vec4 color0 = textureLookup2Dnormalized(colorTex0_, texParams0_, p);
    float depth0 = textureLookup2Dnormalized(depthTex0_, texParams0_, p).z;
    vec4 color1 = textureLookup2Dnormalized(colorTex1_, texParams1_, p);
    float depth1 = textureLookup2Dnormalized(depthTex1_, texParams1_, p).z;
    FragData0 = mix(color0, color1, 0.5);
    gl_FragDepth = min(depth0, depth1);
}
