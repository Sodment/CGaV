#version 330
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
uniform float Time;

void main()
{
    float strength = 0.1;
    vec2 pos = vec2(aTexCoords.x + sin(Time) * strength, aTexCoords.y + cos(Time) * strength);        
    TexCoords = pos;
    //TexCoords = aTexCoords;
    strength = 0.01;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    gl_Position.x += cos(Time * 10) * strength;        
    gl_Position.y += cos(Time * 15) * strength; 
}  