#pragma once

// internal
#include "core/common.hpp"
#include "core/registry.hpp"
#include "core/map.hpp"
#include "core/constants.hpp"
#include "factories.hpp"
#include "gui/gui.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <utility>
#include <entt/entt.hpp>
#include "components/motion.hpp"

enum GameState{
    sandbox,
    level1,
    level2,
    level3,
    level4,
    level5,
    start_screen,
};
// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods]
class Game
{
public:
	// Creates a window
	Game(ivec2 window_size_px);

	// Releases all associated resources
	~Game();


	// restart level
    void restart();

	// Steps the game ahead by ms milliseconds
	void update(float elapsed_ms, vec2 window_size_in_game_units);

	// Check for collisions
    void handle_collisions();

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over() const;

	// OpenGL window handle
	GLFWwindow* window;

	void init_level();
private:
	// Input callback functions
    vec2 get_cursor_position();
    ivec2 get_window_size();
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 mouse_pos);
    void on_mouse_click(int button, int action, int mods);
    void sandbox_on_click(int button, int action, int mods);
    void init_grid();

    // Loads the audio
    void init_audio();

	// Number of fish eaten by the salmon, displayed in the window title
	unsigned int points;

	// Game state
	float current_speed;
    //TODO remove
    float next_turtle_spawn;
	
	// music references
	Mix_Music* background_music;
    //TODO remove
    Mix_Chunk* salmon_dead_sound;
	Mix_Chunk* salmon_eat_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
	MapState mapState;
    GameState game_state = GameState::sandbox;

    void imgui();

    void on_select_sandbox();
};

