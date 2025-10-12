#version 330 core
out vec4 fragColor;

uniform sampler2D texture0;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

void main() {
    vec4 texColor = texture(texture0, TexCoord);
    if (texColor.a < 0.1) discard;
    fragColor = texColor;
}