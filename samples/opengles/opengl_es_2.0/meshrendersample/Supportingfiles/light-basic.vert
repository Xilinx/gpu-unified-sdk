attribute vec3 position;
attribute vec3 normal;

uniform mat4 ModelViewProjectionMatrix;
uniform mat4 NormalMatrix;
uniform vec3 EyePosition;

varying vec3 Normal;
varying vec3 Position;
varying vec3 EyeDirection;
varying vec3 ColorMix;  // New: Pass color mix to fragment shader

void main(void)
{
    // Transform the normal to eye coordinates and normalize it
    Normal = normalize(mat3(NormalMatrix) * normal);

    // Transform the vertex position to eye coordinates
    vec4 eyePosition = ModelViewProjectionMatrix * vec4(position, 1.0);
    Position = vec3(eyePosition);

    // Calculate the eye direction vector
    EyeDirection = normalize(EyePosition - Position);

    // Color interpolation based on position (example mix of three colors)
    vec3 color1 = vec3(1.0, 0.0, 0.0); // Red
    vec3 color2 = vec3(0.0, 1.0, 0.0); // Green
    vec3 color3 = vec3(0.0, 0.0, 1.0); // Blue
    ColorMix = mix(color1, color2, position.y);  // Mix based on Y position
    ColorMix = mix(ColorMix, color3, position.z); // Further mix based on Z position

    // Transform the position to clip coordinates
    gl_Position = ModelViewProjectionMatrix * vec4(position, 1.0);
}
