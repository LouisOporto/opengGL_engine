#version 330 core
out vec4 fragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D depthMap;
uniform samplerCube depthCubeMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float farPlane;

// uniform bool shadows;

// vec3 gridSamplingDisk[20] = vec3[] (
//     vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), 
// );

vec3 calcDirLight();
vec3 calcPointLight();
vec3 calcSpotLight();

float calcShadow(vec3 fragPos) {
    vec3 fragToLight = fragPos - lightPos;
    float closetDepth = texture(depthCubeMap, fragToLight).r;
    closetDepth *= farPlane;
    float currentDepth = length(fragToLight);

    float bias = 0.05;
    float shadow = currentDepth - bias > closetDepth ? 1.0 : 0.0;
    return shadow;
}

#define ambientLight 0.3
#define diffuseLight = 0.5
#define specularLight = 0.1

void main() {
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);

    // Ambient
    vec3 ambient = 0.3 * color;

    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(reflectDir, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    float shadow = calcShadow(fs_in.FragPos);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    // Gamma Correction
    float gamma = 2.0;
    fragColor = vec4(lighting, 1.0);
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / gamma));

    vec3 fragToLight = fs_in.FragPos - lightPos;
    float closetDepth = texture(depthCubeMap, fragToLight).r;
    closetDepth;
    fragColor = vec4(vec3(closetDepth), 1.0);
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