#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} vs_out;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
   vs_out.FragPos = aPos;
    vs_out.Normal = aNormal;
    gl_Position = P * V * M * vec4(aPos, 1.0);
}
