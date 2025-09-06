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

uniform SpotLight spotLight;
uniform DirectionLight dirLight;

#define NUM_OF_POINTS 4
uniform PointLight pointLights[NUM_OF_POINTS];
uniform vec3 viewPos;
uniform bool spotLightOn;
uniform int numPointLights;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

void main() {
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    result += calcDirLight(dirLight, normal, viewDir);

    for (int iter = 0; iter < numPointLights; iter++) 
    {
        result += calcPointLight(pointLights[iter], normal, FragPos, viewDir);
    }

    if (spotLightOn) {
        result += calcSpotLight(spotLight, normal, FragPos, viewDir);
    }

    fragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirectionLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.cutOff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}