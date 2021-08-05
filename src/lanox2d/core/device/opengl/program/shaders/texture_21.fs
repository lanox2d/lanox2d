varying vec4 vColors;
varying vec4 vTexcoords;
uniform sampler2D uSampler;

void main() {
   gl_FragColor = vColors * texture2D(uSampler, vec2(vTexcoords.x, vTexcoords.y));
}


