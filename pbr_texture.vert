#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out VS_OUT {
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

void main()
{
    vs_out.TexCoords = aTexCoords;
    vs_out.WorldPos = vec3(M * vec4(aPos, 1.0));
    vs_out.Normal = mat3(M) * aNormal;   

    gl_Position =   P * V * M * vec4(aPos, 1.0);
}