#pragma once

#include "core/common.hpp"
#include <entt/entt.hpp>

struct Grid
{
    // Creates all the associated render resources and default transform
    // Pass different path of the texture for different girds. e.g., "basic_grid.png" / "mountain.png"
    static entt::entity createGrid(entt::registry& m_registry,vec2 pos, int in_id, std::string texture_path);
};
