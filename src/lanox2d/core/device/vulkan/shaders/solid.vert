#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

precision mediump float;

layout(location = 0) in vec4 aVertices;

layout(binding = 0) uniform uMatrix
{
    mat4 projection;
    mat4 model;
}matrix;

void main() {
   gl_Position = matrix.projection * matrix.model * aVertices;
}
