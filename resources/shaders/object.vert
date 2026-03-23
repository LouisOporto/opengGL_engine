#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;
layout(location=3) in vec4 aTangent;

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
    mat4 lightSpaceMatrix;
};

uniform mat4 model;

out VS_OUT {
    vec3 FragPos;
    vec3 ViewPos;
    vec3 Normal;
    vec2 TexCoord;
    mat3 TBN;
    vec4 FragPosLightSpace;
    vec3 TangentFragPos;
    vec3 TangentViewPos;
} vs_out;

uniform vec3 viewPos;


void main() {
    vec3 T = normalize(vec3(model * vec4(aTangent.xyz, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * aTangent.w;

    vs_out.TBN = transpose(mat3(T, B, N));
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.ViewPos = viewPos;
    vs_out.TexCoord = aTexCoord;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vs_out.TangentFragPos = vs_out.TBN * vs_out.FragPos;
    vs_out.TangentViewPos = vs_out.TBN * viewPos;
}