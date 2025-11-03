#version 330 core
out vec4 fragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    fragColor = vec4(texture(skybox, R).rgb, 1.0);

    // Refract instead
    float ratio = 1.00 / 1.52;
    R = refract(I, normalize(Normal), ratio);
    fragColor =  vec4(texture(skybox, R).rgb, 1.0);
}