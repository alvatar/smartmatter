
#include "modules/mod_sampler2d.frag"

uniform SAMPLER2D_TYPE colorTex_;
uniform SAMPLER2D_TYPE depthTex_;
uniform TEXTURE_PARAMETERS texParams_;

uniform float saturation_;

void main() {
    // lookup input color and depth value (see mod_sampler2d.frag)
    vec4 color = textureLookup2Dscreen(colorTex_, texParams_, gl_FragCoord.xy);
    float depth = textureLookup2Dscreen(depthTex_, texParams_, gl_FragCoord.xy).z;
    
    // compute gray value 
    float brightness = (0.30 * color.r) + (0.59 * color.g) + (0.11 * color.b); 
    vec4 grayscale = vec4(vec3(brightness), color.a);
    FragData0 = mix(grayscale, color, saturation_);
    
    // pass-through depth value
    gl_FragDepth = depth;
}
