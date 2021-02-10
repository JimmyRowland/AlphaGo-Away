#pragma once

#include <vector>

#include "core/common.hpp"
#include <entt/entt.hpp>
#include "core/registry.h"
class AISystem
{
public:
	void step(float elapsed_ms, vec2 window_size_in_game_units);
};
