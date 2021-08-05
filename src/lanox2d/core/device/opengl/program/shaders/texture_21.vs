attribute vec4 aColor;
attribute vec4 aTexcoords;
attribute vec4 aVertices;

varying vec4 vColors;
varying vec4 vTexcoords;
uniform mat4 uMatrixModel;
uniform mat4 uMatrixProject;
uniform mat4 uMatrixTexcoord;

void main() {
   vColors = aColor;
   vTexcoords = uMatrixTexcoord * aTexcoords;
   gl_Position = uMatrixProject * uMatrixModel * aVertices;
}


