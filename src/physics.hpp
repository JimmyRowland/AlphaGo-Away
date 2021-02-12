#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "collision_observer.hpp"
#include "keyboard_observer.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem: public KeyboardObserver
{
public:
	void step(float elapsed_ms, vec2 window_size_in_game_units);

	// Stucture to store collision information
	struct Collision
	{
		// Note, the first object is stored in the ECS container.entities
		ECS::Entity other; // the second object involved in the collision
		Collision(ECS::Entity& other);
	};
	std::vector<CollisionObserver *> collision_observers;
	void on_key_click(int key, int action) override;
private:
    bool should_pause = false;

};
