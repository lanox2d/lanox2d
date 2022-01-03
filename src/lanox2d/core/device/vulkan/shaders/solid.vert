#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

precision mediump float;

//layout(location = 0) in vec4 aColor;
//layout(location = 1) in vec4 aVertices;
layout(location = 0) in vec4 aVertices;

/*layout(location = 0) out vec4 vColors;
layout(binding = 0) uniform uMatrix
{
    mat4 model;
    uniform mat4 projection;
}matrix;
*/

void main() {
//   vColors = aColor;
//   gl_Position = matrix.projection * matrix.model * aVertices;
   gl_Position = aVertices;
}
