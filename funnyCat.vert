#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 gTexCoords;
out vec4 gaPos;


void main()
{
    gTexCoords = aTexCoords;    
    gaPos = vec4(aPos, 1.0);
}