#version 330
in float lifetime;
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D sprite;

void main()
{
	vec4 tex = texture(sprite, texCoord);
	tex.a = tex.a * lifetime;
	FragColor = tex;
   //FragColor = texture(sprite, vec2(1.0 ,0.0));
}