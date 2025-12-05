#version 330 core

layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;

layout(std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

out VS_OUT {
    out vec3 Normal;
    out vec3 FragPos;
} vs_out;

uniform mat4 model;
uniform mat4 inverseModel;

void main() {
    vs_out.Normal =  mat3(transpose(inverseModel)) * aNormal;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}