precision mediump float;

attribute vec4 aColor;
attribute vec4 aVertices;
varying vec4 vColors;
uniform mat4 uMatrixModel;
uniform mat4 uMatrixProject;

void main() {
   vColors = aColor;
   gl_Position = uMatrixProject * uMatrixModel * aVertices;
}

