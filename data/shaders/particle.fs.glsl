#version 330

uniform float time;
// Output color
layout(location = 0) out vec4 color;
in vec2 vl;
void main()
{
	color = vec4(sin(vl.x*time*0.2),sin(vl.y*time*0.2),cos((vl.x+vl.y)*time*0.2), 1.0);
}