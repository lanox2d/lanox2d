#version 440
precision mediump float;

layout(location = 0) in vec4 aColor;
layout(location = 1) in vec4 aTexcoords;
layout(location = 2) in vec4 aVertices;

layout(location = 0) out vec4 vColors;
layout(location = 1) out vec4 vTexcoords;
uniform mat4 uMatrixModel;
uniform mat4 uMatrixProject;
uniform mat4 uMatrixTexcoord;

void main() {
   vColors = aColor;
   vTexcoords = uMatrixTexcoord * aTexcoords;
   gl_Position = uMatrixProject * uMatrixModel * aVertices;
}
