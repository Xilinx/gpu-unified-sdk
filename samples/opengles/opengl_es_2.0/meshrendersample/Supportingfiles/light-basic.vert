attribute vec3 position;
attribute vec3 normal;
attribute vec2 texcoord;

uniform mat4 ModelViewProjectionMatrix;
uniform vec3 EyePosition;

varying vec3 Normal;
varying vec3 Position;
varying vec3 EyeDirection;
varying vec2 TexCoord;

// Lighting is done entirely in the model's object space: positions, normals,
// the light positions and the eye position are all expressed in that same
// space. This keeps everything consistent (the spin is applied only to the
// final clip-space position via ModelViewProjectionMatrix), so the highlights
// stay anchored to the car body as it turns - a clean turntable look.
void main(void)
{
    Normal = normalize(normal);
    Position = position;
    EyeDirection = normalize(EyePosition - Position);
    TexCoord = texcoord;

    gl_Position = ModelViewProjectionMatrix * vec4(position, 1.0);
}
