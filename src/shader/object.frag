#version 330 core
out vec4 fragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

void main() {
    fragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.2);
}