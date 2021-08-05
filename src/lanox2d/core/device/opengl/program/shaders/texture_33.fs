#version 330
precision mediump float;

in vec4 vColors;
in vec4 vTexcoords;
uniform sampler2D uSampler;
out vec4 finalColor;

void main() {
   finalColor = vColors * texture(uSampler, vec2(vTexcoords.x, vTexcoords.y));
}
