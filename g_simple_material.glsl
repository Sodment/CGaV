#version 330
layout(triangles) in;
layout(line_strip) out;
layout(max_vertices = 4) out;

in vec4 gPos[];

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
	int i;
	for (i = 0; i < gl_in.length(); i++)
	{	
		gl_Position = P*V*M*gPos[i];
		EmitVertex();
	}
	gl_Position = P*V*M*gPos[0];
	EmitVertex();
    EndPrimitive();
}