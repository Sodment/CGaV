#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
    FragPos = aPos;
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(M)));
    Normal = normalMatrix * aNormal;

    gl_Position = P * V * M * vec4(aPos, 1.0);
}