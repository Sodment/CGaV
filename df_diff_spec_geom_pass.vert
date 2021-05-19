#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

void main()
{
    FragPos = aPos;
    TexCoords = aTexCoords;
    Normal =  aNormal;
    
    //mat3 normalMatrix = transpose(inverse(mat3(M)));

    gl_Position = P * V * M * vec4(aPos, 1.0);
}