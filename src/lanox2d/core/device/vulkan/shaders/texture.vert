#version 440
precision mediump float;

layout(location = 0) in vec4 aColor;
layout(location = 1) in vec4 aTexcoords;
layout(location = 2) in vec4 aVertices;

layout(location = 0) out vec4 vColors;
layout(location = 1) out vec4 vTexcoords;
layout(binding = 0) uniform uMatrix
{
    mat4 model;
    mat4 projection;
    mat4 texcoord;
}matrix;

void main() {
   vColors = aColor;
   vTexcoords = matrix.texcoord * aTexcoords;
   gl_Position = matrix.projection * matrix.model * aVertices;
}
