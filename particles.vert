#version 330
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec2 offset;
uniform vec4 color;

void main()
{
    float scale = 100.0f;
    TexCoords = vertex.zw;
    ParticleColor = color;
    gl_Position = P *  vec4((vertex.xy * scale) + offset, 0.0, 1.0);
    //gl_Position = P * V * M * vec4(vertex.xy, 0.0, 1.0);
}