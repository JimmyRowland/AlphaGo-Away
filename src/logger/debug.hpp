#pragma once

#include "core/common.hpp"
#include  <entt/entt.hpp>

// will be replaced with imgui

namespace DebugSystem {
	extern bool in_debug_mode;

	// draw a red line for debugging purposes
	void createLine(entt::registry& m_registry, vec2 position, vec2 size);

	// Removes all debugging graphics in ECS, called at every iteration of the game loop
	void clearDebugComponents(entt::registry& m_registry);
};
