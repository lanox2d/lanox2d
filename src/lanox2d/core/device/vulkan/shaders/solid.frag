#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

precision mediump float;

layout(location = 0) in vec4 vColors;
layout(location = 0) out vec4 finalColor;

void main() {
//   finalColor = vColors;
   finalColor = vec4(0.91, 0.26,  0.21, 1.0);
}

