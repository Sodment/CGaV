#version 330

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float amount;

in vec4 gaPos[];
in vec2 gTexCoords[];

out vec2 fTexCoords;

void main()
{
	int i;
	vec3 a = vec3(gaPos[1]) - vec3(gaPos[0]).xyz;
	vec3 b = vec3(gaPos[2]) - vec3(gaPos[0]).xyz;
	vec4 c = vec4(normalize(cross(b,a)), 0);
	vec4 n = normalize(vec4(sin(a.y + b.z), sin(a.y + b.x), sin(b.y + a.z), 0));
	for (i = 0; i < gl_in.length(); i++)
	{	
		gl_Position = P*V*M*(gaPos[i] + amount * n * 5);
		fTexCoords = gTexCoords[i];
		EmitVertex();
	}
	gl_Position = P*V*M*(gaPos[0] + amount * n * 5);
	fTexCoords = gTexCoords[0];
		EmitVertex();
    EndPrimitive();
}
