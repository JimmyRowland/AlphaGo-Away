
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>

// internal
#include "core/common.hpp"
#include "battle_screen_world.hpp"
#include "system/render.hpp"
#include "system/physics.hpp"
#include "ai.hpp"
#include "logger/debug.hpp"

using Clock = std::chrono::high_resolution_clock;

const ivec2 window_size_in_px = {1200, 800};
const vec2 window_size_in_game_units = { 1200, 800 };
// Note, here the window will show a width x height part of the game world, measured in px. 
// You could also define a window to show 1.5 x 1 part of your game world, where the aspect ratio depends on your window size.

struct Description {
	std::string name;
	Description(const char* str) : name(str) {};
};

// Entry point
int main()
{
    entt::registry m_registry;
	// Initialize the main systems
	BattleWorldSystem world(m_registry, window_size_in_px);
	RenderSystem renderer(m_registry, *world.window);
	PhysicsSystem physics;
	AISystem ai;

	// Set all states to default
	world.restart(m_registry);
	auto t = Clock::now();
	// Variable timestep loop
	while (!world.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms = static_cast<float>((std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count()) / 1000.f;
		t = now;

		DebugSystem::clearDebugComponents(m_registry);
		ai.step(elapsed_ms, window_size_in_game_units);
		world.step(m_registry, elapsed_ms, window_size_in_game_units);
		physics.step(m_registry, elapsed_ms, window_size_in_game_units);
		world.handle_collisions(m_registry);

		renderer.draw(m_registry,window_size_in_game_units);
	}
	m_registry.clear<>();

	return EXIT_SUCCESS;
}
