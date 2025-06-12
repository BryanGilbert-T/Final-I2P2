#version 120

varying vec2 v_uv;
uniform sampler2D texture;

uniform vec3 ambient;
uniform int  numLights;
uniform vec2 lightPos[8];
uniform vec3 lightCol[8];
uniform float lightRadius[8];

void main() {
    vec4 baseCol = texture2D(texture, v_uv);
    vec3 outC = baseCol.rgb * ambient;

    vec2 px = gl_FragCoord.xy;
    for(int i = 0; i < numLights; i++) {
        float d   = distance(px, lightPos[i]);
        float att = clamp(1.0 - d/lightRadius[i], 0.0, 1.0);
        outC += baseCol.rgb * lightCol[i] * att;
    }
    gl_FragColor = vec4(outC, baseCol.a);
}
