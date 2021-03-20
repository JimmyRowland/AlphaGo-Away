#version 330

// From vertex shader
in vec2 texcoord;
//in vec4 ParticleColor;

// Application data
uniform sampler2D sampler0;
uniform vec4 particleColor;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = particleColor * texture(sampler0, vec2(texcoord.x, texcoord.y));
}
