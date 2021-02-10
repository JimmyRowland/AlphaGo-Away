#pragma once

#include "core/common.hpp"
#include "core/registry.h"
#include <entt/entt.hpp>
struct Grid
{
    // Creates all the associated render resources and default transform
    // Pass different path of the texture for different girds. e.g., "basic_grid.png" / "mountain.png"
    static entt::entity createGrid(vec2 pos, int in_id, std::string texture_path);
};
