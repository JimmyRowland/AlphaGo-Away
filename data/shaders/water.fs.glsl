#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;

in vec2 texcoord;

layout(location = 0) out vec4 color;

vec2 distort(vec2 uv) 
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //float x = sin(time * 0.1 + uv.x * 12) * 0.007;
    //float y = sin(time * 0.1 + uv.y * 12) * 0.007;
    //uv.x += x;
    //uv.y += y;
    
    //uv.x = max(0.001, uv.x);
    //uv.x = min(0.999, uv.x);
    //uv.y = max(0.001, uv.y);
    //uv.y = min(0.999, uv.y);
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	vec2 coord = uv.xy;
    return coord;
}

vec4 color_shift(vec4 in_color) 
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE THE COLOR SHIFTING HERE (you may want to make it blue-ish)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	//vec4 color = in_color + vec4(-0.1, 0.0, 0.25, 0.0);
	return in_color;
}

vec4 fade_color(vec4 in_color) 
{
	vec4 color = in_color;
	if (darken_screen_factor > 0)
		color -= darken_screen_factor * vec4(0.8, 0.8, 0.8, 0);

	return color;
}

void main()
{
	vec2 coord = distort(texcoord);

    vec4 in_color = texture(screen_texture, coord);
    color = color_shift(in_color);
    color = fade_color(color);
}
