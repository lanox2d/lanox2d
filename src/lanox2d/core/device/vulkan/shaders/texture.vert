#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

precision mediump float;

layout(location = 0) in vec4 aVertices;
layout(location = 1) in vec4 aTexcoords;

layout(binding = 0) uniform uMatrix
{
    mat4 projection;
    mat4 model;
    mat4 texcoord;
}matrix;

layout(location = 1) out vec4 vTexcoords;

void main() {
   vTexcoords = matrix.texcoord * aTexcoords;
   gl_Position = matrix.projection * matrix.model * aVertices;
}
