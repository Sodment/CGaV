#version 330
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
uniform float time;
uniform  float speed;
uniform bool drunk;

void main()
{
    if(drunk)
    {
        float strength = 0.05;
        vec2 pos = vec2(aTexCoords.x + (speed * sin(time) * strength), aTexCoords.y + (speed * cos(time) * strength));        
        TexCoords = pos;
    }
    else
    {
        TexCoords = aTexCoords;
    }
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
}  