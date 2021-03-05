
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>

// internal
#include "core/common.hpp"
#include "core/game.hpp"
#include "system/render.hpp"
#include "system/physics.hpp"
#include "system/ai.hpp"
#include "logger/debug.hpp"
#include "core/constants.hpp"

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// Initialize the main systems
	Game game(window_size_in_px);
	RenderSystem renderer(*game.window);

	// Set all states to default
    game.restart();
	auto t = Clock::now();
	// Variable timestep loop
	while (!game.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms = static_cast<float>((std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count()) / 1000.f;
		t = now;
        DebugSystem::clearDebugComponents();
        aiUpdate(elapsed_ms, window_size_in_game_units);
        game.update(elapsed_ms, window_size_in_game_units);
        physicsUpdate(elapsed_ms, window_size_in_game_units);
        renderer.draw(window_size_in_game_units);
	}
	m_registry.clear<>();

	return EXIT_SUCCESS;
}
