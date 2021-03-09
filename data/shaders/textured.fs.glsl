#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform float darken_factor;

// Output color
layout(location = 0) out  vec4 color;

vec4 fade_color(vec4 in_color)
{
    vec4 color = in_color;
    if (darken_factor > 0)
        color -= darken_factor * vec4(0.8, 0.8, 0.8, 0);

    return color;
}

void main()
{
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
    color = fade_color(color);
}
