precision mediump float;
varying vec2 vTexcoord;
uniform sampler2D checkerTexture;
void main() {
    gl_FragColor = texture2D(checkerTexture, vTexcoord);
}

