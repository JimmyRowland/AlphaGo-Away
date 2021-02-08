#pragma once

#include "core/common.hpp"
#include "tiny_ecs.hpp"

struct Grid
{
    // Creates all the associated render resources and default transform
    // Pass different path of the texture for different girds. e.g., "basic_grid.png" / "mountain.png"
    static ECS::Entity createGrid(vec2 pos, int in_id, std::string texture_path);
};
