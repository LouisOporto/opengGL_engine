#version 330 core

in GS_OUT {
    vec3 Normal;
    vec3 FragPos;
} gs_in;

out vec4 fragColor;

void main() {
    fragColor = vec4(0.0, 1.0, 0.0, 1.0);
}