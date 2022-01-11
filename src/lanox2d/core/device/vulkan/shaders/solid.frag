#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

precision mediump float;

layout(push_constant) uniform PushConsts {
    vec4 aColor;
} pushConsts;

layout(location = 0) out vec4 finalColor;

void main() {
   finalColor = pushConsts.aColor;
}

