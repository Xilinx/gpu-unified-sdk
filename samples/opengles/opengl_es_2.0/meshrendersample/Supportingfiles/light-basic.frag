precision mediump float;

// Key (main) light and a softer fill light from the opposite side. Positions
// are supplied in the same (model) space as the vertex positions.
uniform vec3 LightSourcePosition;   // key light
uniform vec3 FillLightPosition;     // fill light
uniform vec3 MaterialSpecular;
uniform float Shininess;
uniform sampler2D DiffuseTexture;

varying vec3 Normal;
varying vec3 Position;
varying vec3 EyeDirection;
varying vec2 TexCoord;

// Tunable lighting constants.
const vec3  kAmbientColor   = vec3(0.20, 0.21, 0.24); // cool ambient fill
const float kKeyIntensity   = 1.05;
const float kFillIntensity  = 0.35;
const float kRimIntensity   = 0.45;
const vec3  kRimColor       = vec3(0.55, 0.6, 0.75);  // cool rim/sky bounce

void main() {
    // Base (albedo) color from the model's diffuse texture.
    vec3 albedo = texture2D(DiffuseTexture, TexCoord).rgb;

    vec3 normal = normalize(Normal);
    vec3 eyeDir = normalize(EyeDirection);
    vec3 keyDir = normalize(LightSourcePosition - Position);
    vec3 fillDir = normalize(FillLightPosition - Position);

    // Ambient term so shadowed areas keep some of the paint color.
    vec3 ambient = kAmbientColor * albedo;

    // Key + fill diffuse (Lambert).
    float keyDiff = max(dot(normal, keyDir), 0.0);
    float fillDiff = max(dot(normal, fillDir), 0.0);
    vec3 diffuse = (keyDiff * kKeyIntensity + fillDiff * kFillIntensity) * albedo;

    // Specular highlight from the key light (Blinn-Phong half-vector).
    vec3 halfVec = normalize(keyDir + eyeDir);
    float spec = pow(max(dot(normal, halfVec), 0.0), Shininess);
    vec3 specular = spec * MaterialSpecular * kKeyIntensity;

    // Rim light: brighten grazing-angle edges for a glossy, sculpted look.
    float rim = 1.0 - max(dot(normal, eyeDir), 0.0);
    rim = pow(rim, 3.0) * kRimIntensity;
    vec3 rimLight = rim * kRimColor;

    vec3 color = ambient + diffuse + specular + rimLight;

    // Simple gamma correction for punchier, more natural color.
    color = pow(color, vec3(1.0 / 2.2));

    gl_FragColor = vec4(color, 1.0);
}
