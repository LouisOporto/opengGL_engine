#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat4 inverseModel;
uniform sampler2D texture_normal1;
uniform bool NormalOn;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    if (NormalOn) { // This is pointless, because it gives off inaccurate normals
        FragPos = vec3(model * vec4(vec3(texture(texture_normal1, aTexCoord)) + aPos, 1.0));
        Normal = mat3(transpose(inverseModel)) * aNormal;
    }
    else {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverseModel)) * aNormal;
    }
    
    TexCoord = aTexCoord;
}