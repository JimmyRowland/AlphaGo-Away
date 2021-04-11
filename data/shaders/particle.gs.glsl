#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec2 texCoords;
} gs_in[];

out vec2 TexCoords;

uniform float time;

vec4 explode(vec4 position)
{
	float velocity = 1.0;
	vec2 dir = vec2(velocity)*time;
	return position + vec4(dir, 1.0,1.0);
}

void main() {
	gl_Position = explode(gl_in[0].gl_Position);
	TexCoords = gs_in[0].texCoords;
	EmitVertex();
	gl_Position = explode(gl_in[1].gl_Position);
	TexCoords = gs_in[1].texCoords;
	EmitVertex();
	gl_Position = explode(gl_in[2].gl_Position);
	TexCoords = gs_in[2].texCoords;
	EmitVertex();
	EndPrimitive();
}