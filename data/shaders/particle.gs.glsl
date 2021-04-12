#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 texCoords;
} gs_in[];

out vec2 TexCoords;

uniform float time;
uniform float particle_explosion_time;
float hash(float x)
{
    return fract(sin(x*.0127863)*17143.321);
}
vec4 explode(vec4 position, vec2 velocity)
{
    vec2 dir = velocity*(2.0-particle_explosion_time+time);
    return position + vec4(dir, 0.0, 0.0);
}

void explosion(){
    vec2 center = (gs_in[0].texCoords.xy + gs_in[1].texCoords.xy + gs_in[2].texCoords.xy);
    vec2 hash_velocity = (vec2(hash(center.x), hash(center.y))-0.5) * 2;
    //	vec2 hash_velocity = center;
    gl_Position = explode(gl_in[0].gl_Position, hash_velocity);
    //	gl_Position = gl_in[0].gl_Position;
    TexCoords = gl_Position.xy;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, hash_velocity);
    //	gl_Position = gl_in[1].gl_Position;

    TexCoords = gl_Position.xy;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, hash_velocity);
    //	gl_Position = gl_in[2].gl_Position;

    TexCoords = gl_Position.xy;
    EmitVertex();
    EndPrimitive();
}

void move(){

    gl_Position = gl_in[0].gl_Position;
    TexCoords = gl_Position.xy;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;

    TexCoords = gl_Position.xy;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;

    TexCoords = gl_Position.xy;
    EmitVertex();
    EndPrimitive();
}

void main() {
    if (particle_explosion_time-time>0){
        explosion();
    } else {
        move();
    }
}