#version 330 core

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D screenTexture;

void main() {
    fragColor = texture(screenTexture, TexCoords);
    // Inversion
    // fragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);

    // Grayscale
    // fragColor = texture(screenTexture, TexCoords);
    // float average = (fragColor.r + fragColor.g + fragColor.b) / 3.0;
    // fragColor = vec4(average, average, average, 1.0);

    // Grayscale (Weighted channels)
    fragColor = texture(screenTexture, TexCoords);
    float average = (0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b) / 3.0;
    fragColor = vec4(average, average, average, 1.0);
}