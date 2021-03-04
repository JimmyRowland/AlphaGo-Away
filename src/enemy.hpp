#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Enemy
{
    // Creates all the associated render resources and default transform
    // Pass different path of the texture for different enemies. e.g., "ai_long-d.png" / "ai_recover.png"
    static ECS::Entity createEnemy(vec2 position, std::string texture_path, vec2 size, vec2 coor);
};
