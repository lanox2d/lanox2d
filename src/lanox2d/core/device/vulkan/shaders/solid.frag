#version 440
precision mediump float;

layout(location = 0) in vec4 vColors;
layout(location = 0) out vec4 finalColor;

void main() {
   finalColor = vColors;
}

