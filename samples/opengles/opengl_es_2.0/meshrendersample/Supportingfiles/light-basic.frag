precision mediump float;

uniform vec3 LightSourcePosition;
uniform vec3 MaterialSpecular;
uniform float Shininess;

varying vec3 Normal;
varying vec3 Position;
varying vec3 EyeDirection;
varying vec3 ColorMix;  // New: Receive color mix from vertex shader

void main() {
    // Normalize the normal, light direction, and eye direction
    vec3 lightDir = normalize(LightSourcePosition - Position);
    vec3 normal = normalize(Normal);
    vec3 eyeDir = normalize(EyeDirection);

    // Ambient component
    vec3 ambient = 0.008 * ColorMix;

    // Diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * ColorMix;

    // Specular component
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, eyeDir), 0.0), Shininess);
    vec3 specular = spec * MaterialSpecular;

    // Emissive (glow) component (can be adjusted as needed)
    vec3 emissive = vec3(0.09, 0.01, 0.01);  // Very subtle red glow

    // Combine all components
    vec3 color = ambient + diffuse + specular + emissive;
    gl_FragColor = vec4(color, 1.0);
}
