#pragma once

#include <vector>

#include "core/common.hpp"

class AISystem
{
public:
	void step(float elapsed_ms, vec2 window_size_in_game_units);
};
