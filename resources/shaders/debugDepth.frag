#version 330 core

uniform sampler2D depthMap;

in vec2 TexCoords;

out vec4 fragColor;

void main() {
    float depthValue = texture(depthMap, TexCoords).r;
    fragColor = vec4(vec3(depthValue), 1.0); 
}