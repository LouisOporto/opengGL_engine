#version 330 core 
layout(points) in;
layout(line_strip, max_vertices = 2) out;

in VS_OUT {
    vec3 Normal;
    vec3 FragPos;
} vs_in[];

out GS_OUT {
    vec3 Normal;
    vec3 FragPos;
} gs_out;

void main() {
    gs_out.Normal = vs_in[0].Normal;
    gs_out.FragPos = vs_in[0].FragPos;
    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
