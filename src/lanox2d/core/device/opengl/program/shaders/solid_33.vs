#version 330
precision mediump float;

layout(location = 0) in vec4 aColor;
layout(location = 1) in vec4 aVertices;
uniform mat4 uMatrixModel;
uniform mat4 uMatrixProject;
out vec4 vColors;

void main() {
   vColors = aColor;
   gl_Position = uMatrixProject * uMatrixModel * aVertices;
}
