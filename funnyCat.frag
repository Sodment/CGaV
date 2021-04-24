#version 330
out vec4 FragColor;

in vec2 fTexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
    FragColor = texture(texture_diffuse1, fTexCoords);
}