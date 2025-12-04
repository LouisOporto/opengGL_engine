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

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;

    bool missingDiffuse;
    bool missingSpecular;
    bool missingNormal;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

vec3 calcDirLight(DirectionLight light, vec3 norm, vec3 viewDir, vec4 texColor);
vec3 calcPointLight(PointLight light, vec3 norm, vec3 viewDir, vec4 texColor, vec3 fragPos);
vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 viewDir, vec4 texColor, vec3 fragPos);

uniform Material material;

uniform SpotLight spotLight;
uniform DirectionLight dirLight;

#define NUM_OF_POINTS 4
uniform PointLight pointLights[NUM_OF_POINTS];
uniform vec3 viewPos;
uniform bool spotLightOn;
uniform int numPointLights;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    mat3 TBN;
} vs_in;

void main() {
    vec4 texColor;
    vec3 normal = normalize(vs_in.Normal);
    vec3 viewDir = normalize(viewPos - vs_in.FragPos);
    vec3 result = vec3(0.0);

    if (material.missingDiffuse) {
        texColor = vec4(material.diffuse, 1.0);
    }
    else {
        texColor = texture(material.texture_diffuse1, vs_in.TexCoord);
    }

    result += calcDirLight(dirLight, normal, viewDir, texColor);

    for (int iter = 0; iter < numPointLights; iter++) {
        result += calcPointLight(pointLights[iter], normal, viewDir, texColor, vs_in.FragPos);
    }

    if (spotLightOn) {
        result += calcSpotLight(spotLight, normal, viewDir, texColor, vs_in.FragPos);
    }

    fragColor =  vec4(result, 1.0);
}

// No ambience for now
vec3 calcDirLight(DirectionLight light, vec3 norm, vec3 viewDir, vec4 texColor) {
    vec3 lightDir = normalize(-light.direction);

    vec3 ambient = light.ambient * material.ambient * vec3(texColor);

    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * diff * vec3(texColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec3 specular = light.specular * material.specular * spec * vec3(texture(material.texture_specular1, vs_in.TexCoord));

    if (material.missingSpecular) {
        specular = light.specular * material.specular * spec;
    }

    if (material.shininess == 0.0) {
        specular = vec3(0.0);
    }

    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 norm, vec3 viewDir, vec4 texColor, vec3 fragPos) {
    vec3 lightDir = normalize(light.position - fragPos);

    vec3 ambient = light.ambient * material.ambient * vec3(texColor);

    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * diff * vec3(texColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec3 specular = light.specular * material.specular * spec * vec3(texture(material.texture_specular1, vs_in.TexCoord));

    if (material.missingSpecular) {
        specular = light.specular * material.specular * spec;
    }
    
    if (material.shininess == 0.0) {
        specular = vec3(0.0);
    }
    
    float distance = length(light.position - fragPos);
    float attenuation =  1 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 viewDir, vec4 texColor, vec3 fragPos) {
    vec3 lightDir = normalize(light.position - fragPos);

    vec3 ambient = light.ambient * material.ambient * vec3(texColor);

    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * diff * vec3(texColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec3 specular = light.specular * material.specular *  spec * vec3(texture(material.texture_specular1, vs_in.TexCoord));

    if (material.missingSpecular) {
        specular = light.specular * material.specular * spec;
    }

    if (material.shininess == 0.0) {
        specular = vec3(0.0);
    }
    
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.cutOff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;
    
    float distance = length(light.position - fragPos);
    float attenuation =  1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}