#version 120

attribute vec4 al_pos;
attribute vec2 al_texcoord;
uniform mat4 al_projview_matrix;

// this must match your FS
varying vec2 v_uv;

void main() {
    v_uv = al_texcoord;
    gl_Position = al_projview_matrix * al_pos;
}