#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;
layout(location=3) in vec3 aTangent;

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
    mat4 lightSpaceMatrix;
};

struct MaterialVert {
    sampler2D texture_height1;
    sampler2D texture_normal1;

    bool missingHeight;
    bool missingNormal;
};

uniform mat4 model;
uniform MaterialVert materialVert;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    mat3 TBN;
    vec4 FragPosLightSpace;
} vs_out;

// out VS_OUT { // Explosion using geom
//     vec2 texCoords;
// } vs_out;

void main() {
    mat3 normalMatrix = mat3(transpose(inverse(model)));

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    vs_out.TBN = mat3(T, B, N);

    if (materialVert.missingNormal) {
        vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
        // vs_out.Normal = normalMatrix * aNormal;
    }
    else {
        vs_out.Normal = texture(materialVert.texture_normal1, aTexCoord).rgb;
        vs_out.Normal = vs_out.Normal * 2.0 - 1.0;
        vs_out.Normal = normalize(vs_out.TBN * vs_out.Normal);
    }

    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoord = aTexCoord;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}