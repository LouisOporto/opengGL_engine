#version 330 core

uniform sampler2D depthMap;

in vec2 TexCoords;

uniform float nearPlane;
uniform float farPlane;

out vec4 fragColor;

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main() {
    // float depthValue = texture(depthMap, TexCoords).r;
    // fragColor = vec4(vec3(linearizeDepth(depthValue) / farPlane), 1.0);
    float depthValue = texture(depthMap, TexCoords).r;
    fragColor = vec4(vec3(depthValue), 1.0);
}