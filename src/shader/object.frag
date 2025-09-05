#version 330 core
out vec4 fragColor;

struct DirectionLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
};

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

vec3 calcDirLight(DirectionLight light, vec3 normal, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform Material material;
uniform SpotLight spotlight;
uniform DirectionLight dirLight;
uniform PointLight pointLights;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

void main() {
    fragColor = mix(texture(material.diffuse, TexCoord), texture(material.specular, TexCoord), 0.2);
}