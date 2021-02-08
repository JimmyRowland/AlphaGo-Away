#pragma once

#include "core/common.hpp"
#include <entt/entt.hpp>

#include "factory.hpp"

class TurtleFactory: public Factory
{
public:
    TurtleFactory(entt::registry & registry);
	entt::entity createTurtle(vec2 position);
};
