#version 440
precision mediump float;

layout(location = 0) in vec4 aColor;
layout(location = 1) in vec4 aVertices;

layout(location = 0) out vec4 vColors;
layout(binding = 0) uniform uMatrix
{
    mat4 model;
    uniform mat4 projection;
}matrix;

void main() {
   vColors = aColor;
   gl_Position = matrix.projection * matrix.model * aVertices;
}
