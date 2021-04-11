#version 330
#define max_particle 20
uniform sampler2D screen_texture;
uniform float time;
uniform int dark_mode;
uniform float darken_screen_factor;
uniform float particle_x[max_particle];
uniform float particle_y[max_particle];
in vec2 texcoord;


layout(location = 0) out vec4 color;

/*by musk License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

	messing around with my previous shader . . .


*/

//bounce count drasticaly reduces performance for long simulations
//setting them to zero disables bouncing but drastically improves framerate
#define max_floor_bounce 4
#define max_wall_bounce 8



//brightness conntrols the how much light each particle emits
//reduce brightness if you put in more particles
#define brightness 6.6

//how many seconds you want each simulation to run
//try with 4.0, 2.0, 1.0, 0.5, 0.33, 0.2
#define time_per_simulation 4.0

//time offset to get a nice screenshot ;)
#define time_offset -5.75

//solve 2nd order polynomial, return the maximum of two solutions
//could have optimised for this particular example... meh...
float second(float a,float b,float c)
{
	float x1 = (-b+sqrt((b*b-4.0*a*c)))/(2.0*a);
	float x2= (-b-sqrt((b*b-4.0*a*c)))/(2.0*a);
	return max(x1,x2);
}

float hash(float x)
{
	return fract(sin(x*.0127863)*17143.321); //decent hash for noise generation
}

float hash(vec2 x)
{
	return fract(cos(dot(x.xy,vec2(0.37311,53.21))*121.1126143)*412.0);
}

vec3 cc(vec3 color, float factor,float factor2) // color modifier
{
	float w = color.x+color.y+color.z;
	return mix(color,vec3(w)*factor,w*factor2);
}

vec4 mainImage( vec2 uv )
{



	float acc = .0;

	float t = time + time_offset;



	for (int sampleIndex = 0; sampleIndex<max_particle; sampleIndex++)
	{
		if(particle_x[sampleIndex] <= -1) break;
		vec2 p = vec2((particle_x[sampleIndex]/1600.0-0.5), (0.5-particle_y[sampleIndex]/800.0));
		vec2 temp = (((uv-0.5))-p.xy)*100.0;
		temp/=800.0/300.0;
		float s = sqrt(dot(temp,temp));
		s*=800.0*.05;
		acc+=1.0/(s+1.0);
	}

	vec3 part_color = vec3(1.0,1.0,1.0);
	part_color = normalize(abs(part_color));
	vec3 color = vec3(.01,.01,.01)+part_color*acc*brightness;


	color *=.8;
	color -= length(uv)*.005;
	color = cc(color,.5,.5);
	color = vec3(1.2)*color/(vec3(1.0)+color);
	color += hash(uv.xy+color.xy)*.02;

	vec4 fragColor = vec4(color,1.0);
	return fragColor;
}





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
	if(texcoord.x<0.75 && dark_mode > 0){
		color *= mainImage(texcoord);
	}

}
