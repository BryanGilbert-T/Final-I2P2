#version 100

precision mediump float;
varying vec4 varying_color;
varying vec2 varying_texcoord;
uniform sampler2D al_tex;
uniform sampler2D light_tex;
uniform bool use_texture;
uniform bool use_light;
void main() {
    vec4 c;
    if (use_texture) {
        c = texture2D(al_tex, varying_texcoord);
    } else {
        c = varying_color;
    }
    if (use_light) {
        vec4 l = texture2D(light_tex, varying_texcoord);
        c.rgb *= l.rgb * l.a;
    }
    gl_FragColor = c;
}
