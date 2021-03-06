
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>

// internal
#include "ai.hpp"
#include "battle_screen_world.hpp"
#include "common.hpp"
#include "debug.hpp"
#include "physics.hpp"
#include "render.hpp"
#include "tiny_ecs.hpp"
#include "level_state.hpp"


using Clock = std::chrono::high_resolution_clock;

// TODO: Since we have > 1 world systems, we can set different window size here
const ivec2 window_size_in_px = {1000, 700};
const vec2 window_size_in_game_units = { 1000, 700 };
// grid_dim is {grid width, number grid squares in x dim, number grid squares in y dim}
const std::tuple<float, int, int> grid_dim = std::make_tuple(580.f, 10, 10);
// Note, here the window will show a width x height part of the game world, measured in px. 
// You could also define a window to show 1.5 x 1 part of your game world, where the aspect ratio depends on your window size.

struct Description {
	std::string name;
	Description(const char* str) : name(str) {};
};

// Entry point
int main()
{
	// Initialize the main systems
    // StartWorldSystem start_world(window_size_in_px);
    LevelStateSystem levelState(100, 0, 0);
    UnitFactory unitFactory(levelState);
    BattleWorldSystem battle_world(window_size_in_px, unitFactory, grid_dim);
	// RenderSystem renderer(*start_world.window);
	RenderSystem renderer(*battle_world.window);
	PhysicsSystem physics;
    physics.collision_observers.push_back([&](ECS::Entity a, ECS::Entity b) {return levelState.on_collision(a, b);});
    physics.collision_observers.push_back([&](ECS::Entity a, ECS::Entity b) {return battle_world.on_collision(a, b);});
    
    battle_world.keyBoardObservers.push_back(&unitFactory);
    battle_world.keyBoardObservers.push_back(&physics);
    battle_world.physicsSystem = &physics;

	AISystem ai;

	// Set all states to default
    battle_world.restart();
	auto t = Clock::now();
	// Variable timestep loop
	while (!battle_world.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms = static_cast<float>((std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count()) / 1000.f;
		t = now;

		DebugSystem::clearDebugComponents();
		ai.step(elapsed_ms, window_size_in_game_units, grid_dim);
        battle_world.step(elapsed_ms, window_size_in_game_units);
		physics.step(elapsed_ms, window_size_in_game_units, grid_dim);
        battle_world.handle_collisions();

		renderer.draw(window_size_in_game_units);
	}

	return EXIT_SUCCESS;
}
