#version 330 core

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;
void main() {
    fragColor = texture(screenTexture, TexCoords);
    // Inversion
    // fragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);

    // Grayscale
    // fragColor = texture(screenTexture, TexCoords);
    // float average = (fragColor.r + fragColor.g + fragColor.b) / 3.0;
    // fragColor = vec4(average, average, average, 1.0);

    // Grayscale (Weighted channels)
    // fragColor = texture(screenTexture, TexCoords);
    // float average = (0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b) / 3.0;
    // fragColor = vec4(average, average, average, 1.0);

    
    // vec2 offsets[9] = vec2[](
    //     vec2(-offset, offset),
    //     vec2( 0.0f,   offset),
    //     vec2( offset, offset),
    //     vec2(-offset, 0.0f),
    //     vec2( 0.0f,   0.0f),
    //     vec2( offset, 0.0f),
    //     vec2(-offset, -offset),
    //     vec2( 0.0f,   -offset),
    //     vec2( offset, -offset)
    // );

    // float kernel[9] = float[](
    //     -1, -1, -1,
    //     -1,  9, -1,
    //     -1, -1, -1
    // );

    // float kernel[9] = float[](
    //     1.0 / 16, 2.0 / 16, 1.0 / 16,
    //     2.0 / 16, 4.0 / 16, 2.0 / 16,
    //     1.0 / 16, 2.0 / 16, 1.0 / 16
    // );

    // float kernel[9] = float[](
    //     1, 1, 1,
    //     1,-8, 1,
    //     1, 1, 1
    // );

    // vec3 sampleTex[9];
    // for(int i = 0; i < 9; i++) {
    //     sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    // }

    // vec3 col = vec3(0.0);
    // for (int i = 0; i < 9; i++) {
    //     col += sampleTex[i] * kernel[i];
    // }

    // fragColor = vec4(col , 1.0);
}