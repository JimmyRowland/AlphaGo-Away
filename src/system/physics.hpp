#pragma once

#include "core/common.hpp"
#include "core/registry.h"
#include <entt/entt.hpp>
// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void step(float elapsed_ms, vec2 window_size_in_game_units);

	// Stucture to store collision information
	struct Collision
	{
		// Note, the first object is stored in the ECS container.entities
		entt::entity other; // the second object involved in the collision
		Collision(entt::entity& other);
	};
};
