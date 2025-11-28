#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;
layout(location=3) in vec3 aTangent;

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

struct MaterialVert {
    sampler2D texture_height1;
    sampler2D texture_normal1;

    bool missingHeight;
    bool missingNormal;
};

uniform mat4 model;
uniform mat4 inverseModel;
uniform MaterialVert materialVert;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out mat3 TBN;

void main() {
    mat3 normalMatrix = mat3(transpose(inverseModel));

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);

    if (materialVert.missingNormal) {
        Normal = mat3(transpose(model)) * aNormal;
    }
    else {
        Normal = texture(materialVert.texture_normal1, aTexCoord).rgb;
        Normal = Normal * 2.0 - 1.0;
        Normal = normalize(TBN * Normal);
    }

    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoord = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}