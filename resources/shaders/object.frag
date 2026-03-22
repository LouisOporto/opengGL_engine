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
    sampler2D texture_shininess1;
    sampler2D texture_normal1;
    sampler2D texture_height1;

    bool missingDiffuse;
    bool missingSpecular;
    bool missingShininess;
    bool missingNormal;
    bool missingHeight;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

vec3 calcDirLight(DirectionLight light, vec3 norm, vec3 viewDir, vec4 texColor, vec2 texCoord);
vec3 calcPointLight(PointLight light, vec3 norm, vec3 viewDir, vec4 texColor, vec2 texCoord, vec3 fragPos);
vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 viewDir, vec4 texColor, vec2 texCoord, vec3 fragPos);
float calcShadow(vec4 FragPosLightSpace);
vec2 calcParllax(vec2 texCoords, vec3 viewDir);

uniform Material material;
uniform sampler2D depthMap;
uniform bool usingDepth;
uniform float heightScale;

uniform SpotLight spotLight;
uniform DirectionLight dirLight;

#define NUM_OF_POINTS 4
uniform PointLight pointLights[NUM_OF_POINTS];
uniform bool spotLightOn;
uniform int numPointLights;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    mat3 TBN;
    vec4 FragPosLightSpace;
    vec3 TangentFragPos;
    vec3 TangentViewPos;
} fs_in;

float linearizeDepth(float depth) {
    float value = depth * 2.0 - 1.0;
    return (2.0 * 0.1 * 200.0) / (200.0 + 0.1 - value * (200.0 - 0.1)); // Formula : (2 * nearZ * farZ) / (farZ + nearZ - 2 * (farZ - nearZ))
}

float calcShadow(vec4 fragPosLightSpace, vec3 norm, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    // return projCoords.x;
    float closetDepth = texture(depthMap, projCoords.xy).r;
    // closetDepth =  linearizeDepth(closetDepth) / 200.0;
    float currentDepth = projCoords.z;
    // return currentDepth;
    float bias = max(0.02 * (1.0 - dot(norm, lightDir)), 0.005);
    // bias = 0.005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // float shadow = currentDepth - bias > closetDepth ? 1.0 : 0.0;
    // return shadow;
    if (projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

vec2 calcParllax(vec2 texCoords, vec3 viewDir) {
    // float height = texture(material.texture_height1, texCoords).r;
    // return texCoords - viewDir.xy * (height * heightScale);

    const float numLayers = 10;
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy * heightScale;
    vec2 deltaTexCoords = P / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue) {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    return currentTexCoords;
}

void main() {
    vec4 texColor;
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoord = fs_in.TexCoord;

    texCoord.y = 1.0 - texCoord.y; // Flip because flipUVs disabled in assimp

    if (material.missingDiffuse) {
        texColor = vec4(material.diffuse, 1.0);
    } else {
        if (!material.missingHeight) {
            texCoord = calcParllax(texCoord, viewDir);
            if (texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0) discard;
        }
        texColor = texture(material.texture_diffuse1, texCoord);
    }

    if (!material.missingNormal) {
        normal = texture(material.texture_normal1, texCoord).rgb;
        normal = normalize(normal * 2.0 - 1.0);
    }

    vec3 result = vec3(0.0);

    result += calcDirLight(dirLight, normal, viewDir, texColor, texCoord);
    for (int iter = 0; iter < numPointLights; iter++) {
        result += calcPointLight(pointLights[iter], normal, viewDir, texColor, texCoord, fs_in.TangentFragPos);
    }

    if (spotLightOn) {
        result += calcSpotLight(spotLight, normal, viewDir, texColor, texCoord, fs_in.TangentFragPos);
    }

    float gamma = 2.2;
    fragColor =  vec4(result, 1.0);
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
}

// No ambience for now
vec3 calcDirLight(DirectionLight light, vec3 norm, vec3 viewDir, vec4 texColor, vec2 texCoord) {
    vec3 lightDir = normalize(light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 ambient = light.ambient * material.ambient * vec3(texColor);

    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * diff * vec3(texColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, halfwayDir), 0.0), float(texture(material.texture_shininess1, texCoord)) * 256.f);
    if (material.missingShininess) {
        spec = pow(max(dot(reflectDir, halfwayDir), 0.0), material.shininess);
    }

    vec3 specular = light.specular * material.specular * spec * vec3(texture(material.texture_specular1, texCoord));
    if (material.missingSpecular) {
        specular = light.specular * material.specular * spec;
        if (material.shininess == 0.0) {
            specular = vec3(0.0);
        }
    }

    float shadow = 0.0;
    if (usingDepth) {
        shadow = calcShadow(fs_in.FragPosLightSpace, norm, lightDir);
    } 
    // return vec3(shadow);
    return (ambient + (1.0 - shadow) * (diffuse + specular));
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 norm, vec3 viewDir, vec4 texColor, vec2 texCoord, vec3 fragPos) {
    vec3 lightDir = normalize(fs_in.TBN * light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 ambient = light.ambient * material.ambient * vec3(texColor);

    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * diff * vec3(texColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, halfwayDir), 0.0), float(texture(material.texture_shininess1, texCoord)) * 256.f);
    if (material.missingShininess) {
        spec = pow(max(dot(reflectDir, halfwayDir), 0.0), material.shininess);
    }

    vec3 specular = light.specular * material.specular * spec * vec3(texture(material.texture_specular1, texCoord));
    if (material.missingSpecular) {
        specular = light.specular * material.specular * spec;
        if (material.shininess == 0.0) {
            specular = vec3(0.0);
        }
    }
    
    float distance = length(light.position - fragPos);
    float attenuation =  1 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 viewDir, vec4 texColor, vec2 texCoord, vec3 fragPos) {
    vec3 lightDir = normalize(fs_in.TBN * light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 ambient = light.ambient * material.ambient * vec3(texColor);

    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * diff * vec3(texColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, halfwayDir), 0.0), float(texture(material.texture_shininess1, texCoord)) * 256.f);
    if (material.missingShininess) {
        spec = pow(max(dot(reflectDir, halfwayDir), 0.0), material.shininess);
    }

    vec3 specular = light.specular * material.specular * spec * vec3(texture(material.texture_specular1, texCoord));
    if (material.missingSpecular) {
        specular = light.specular * material.specular * spec;
        if (material.shininess == 0.0) {
            specular = vec3(0.0);
        }
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