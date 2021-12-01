#version 440
precision mediump float;

layout(location = 0) in vec4 vColors;
layout(location = 1) in vec4 vTexcoords;

layout(location = 0) out vec4 finalColor;
layout(binding = 0) uniform sampler2D uSampler;

void main() {
   finalColor = vColors * texture(uSampler, vec2(vTexcoords.x, vTexcoords.y));
}
