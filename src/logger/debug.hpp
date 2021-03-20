#ifndef ALPHAGO_AWAY_DEBUG_HPP
#define ALPHAGO_AWAY_DEBUG_HPP
#include "core/common.hpp"
#include "core/registry.hpp"
#include  <entt/entt.hpp>
#include "components/motion.hpp"
// will be replaced with imgui

namespace DebugSystem {
	extern bool in_debug_mode;

	// draw a red line for debugging purposes
    void createLine(vec2 position, vec2 size);

	// Removes all debugging graphics in ECS, called at every iteration of the game loop
    void clearDebugComponents();
};
#endif //ALPHAGO_AWAY_DEBUG_HPP