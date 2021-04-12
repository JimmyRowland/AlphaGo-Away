#version 330
// Output color
layout(location = 0) out vec4 color;
in vec2 vl;
void main()
{
	color = vec4(sin(vl.x*9),sin(vl.y*9),cos((vl.x+vl.y)*9), 1.0);
}