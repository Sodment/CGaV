#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec4 gPos;

void main()
{
	gPos = vec4(aPos, 1);
}