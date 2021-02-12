#include "common.hpp"
#include <iostream>

// Note, we could also use the functions from GLM but we write the transformations here to show the uderlying math
void Transform::scale(vec2 scale)
{
	mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * S;
}

void Transform::rotate(float radians)
{
	float c = std::cos(radians);
	float s = std::sin(radians);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * R;
}

void Transform::translate(vec2 offset) {
    mat3 T = {{1.f,      0.f,      0.f},
              {0.f,      1.f,      0.f},
              {offset.x, offset.y, 1.f}};
    mat = mat * T;
}
float get_velocity_after_drag(float velocity_x){
    if(velocity_x > 0){
        float new_velocity_x = velocity_x - 2.f;
        return new_velocity_x > 0 ? new_velocity_x : 0;
    }else{
        float new_velocity_x = velocity_x + 2.f;
        return new_velocity_x < 0 ? new_velocity_x : 0;
    }
}