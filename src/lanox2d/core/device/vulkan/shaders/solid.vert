#version 440
precision mediump float;

layout(location = 0) in vec4 aColor;
layout(location = 1) in vec4 aVertices;

layout(location = 0) out vec4 vColors;
uniform mat4 uMatrixModel;
uniform mat4 uMatrixProject;

void main() {
   vColors = aColor;
   gl_Position = uMatrixProject * uMatrixModel * aVertices;
}
