#version 330 core
out vec4 fragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
// uniform samplerCube depthMap;

// uniform vec3 lightPos;
// uniform vec3 viewPos;

// uniform float farPlane;
// uniform bool shadows;

// vec3 gridSamplingDisk[20] = vec3[] (
//     vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), 
// );

vec3 calcDirLight();
vec3 calcPointLight();
vec3 calcSpotLight();

float calcShadow(vec3 fragPos) {
    float shadow = 0.0;
    return shadow;
}

#define ambientLight 0.3
#define diffuseLight = 0.5
#define specularLight = 0.1

void main() {
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);

    vec3 ambient = lightColor;

    vec3 lighting = color;
    fragColor = vec4(lighting, 1.0);
}

vec3 calcDirLight() {
    return vec3(0.0);
}

vec3 calcPointLight() {
    return vec3(0.0);
}

vec3 calcSpotLight() {
    return vec3(0.0);
}