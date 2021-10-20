#version 330
precision mediump float;

in vec4 aColor;
in vec4 aTexcoords;
in vec4 aVertices;

out vec4 vColors;
out vec4 vTexcoords;
uniform mat4 uMatrixModel;
uniform mat4 uMatrixProject;
uniform mat4 uMatrixTexcoord;

void main() {
   vColors = aColor;
   vTexcoords = uMatrixTexcoord * aTexcoords;
   gl_Position = uMatrixProject * uMatrixModel * aVertices;
}
