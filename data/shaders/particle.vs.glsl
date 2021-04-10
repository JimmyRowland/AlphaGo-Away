
	#version 330

// Input attributes
in vec3 in_color;
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

out vec3 vcolor;

// Application data
uniform mat3 transform;
uniform mat3 projection;
layout (location = 0) in vec3 aOffset;
void main()
{
	texcoord = in_texcoord;
	vcolor = in_color;
	mat3 offset = mat3(
	aOffset.z, 0.0, 0.0,
	0.0, aOffset.z, 0.0,
	aOffset.x, aOffset.y, 1.0);
	vec3 pos = projection * (offset) * vec3(in_position.xy, 1.0); // why not simply *in_position.xyz ?
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}