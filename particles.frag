#version 330
in float lifetime;
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D sprite;

void main()
{
   FragColor = texture(sprite, texCoord);
   //FragColor = texture(sprite, vec2(1.0 ,0.0));
}