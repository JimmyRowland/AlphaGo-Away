#version 330 

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;
//out vec4 ParticleColor;

// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
	texcoord = in_texcoord;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
//    ParticleColor = color;
//    gl_Position = projection * vec4((vertex.xy * scale) + offset, 0.0, 1.0);
}
