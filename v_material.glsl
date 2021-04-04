#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
    FragPos = vec3(M * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(M))) * aNormal;  
    gl_Position = P * V * vec4(FragPos, 1.0);
}
