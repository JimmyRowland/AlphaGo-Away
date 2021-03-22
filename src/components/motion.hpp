//
// Created by grumpy on 2021-03-05.
//

#ifndef ALPHAGO_AWAY_MOTION_HPP
#define ALPHAGO_AWAY_MOTION_HPP
#include "core/common.hpp"
struct Motion {
    vec2 velocity = { 0, 0 };
    vec2 acceleration {0,0};
};

struct Position {
    vec2 position = { 0, 0 };
    float angle = 0;
    vec2 scale = { 10, 10 };
};
struct BoundingBox {
    std::vector<vec2> vertices;
    std::vector<vec2> transformed_vertices;
    void scale(vec2 scale);
    void rotate(float radians);
    void translate(vec2 offset);
};

struct Projectiles {
    entt::entity pro;
};
#endif //ALPHAGO_AWAY_MOTION_HPP
