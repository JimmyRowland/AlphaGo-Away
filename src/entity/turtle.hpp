#pragma once

#include "core/common.hpp"
#include <entt/entt.hpp>

struct Turtle{
    static 	entt::entity createTurtle(entt::registry& m_registry,vec2 position);
};

