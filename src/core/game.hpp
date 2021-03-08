#pragma once

// internal
#include "core/common.hpp"
#include "core/registry.hpp"
#include "core/map.hpp"
#include "core/constants.hpp"
#include "factories.hpp"
#include "gui/gui.hpp"
#include "system/physics.hpp"
#include "logger/debug.hpp"
#include "system/render_components.hpp"

// stlib
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <iomanip>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <utility>
#include <entt/entt.hpp>
#include "components/motion.hpp"
#include <nlohmann/json.hpp>

enum Level{
    sandbox,
    level1,
    level2,
    level3,
    level4,
    level5,
    start_screen,
};

struct ImguiState{
//    PlaceTileType place_tile_type = PlaceTileType::disabled;
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
    void init_map_grid();

    // Loads the audio
    void init_audio();

	// Number of fish eaten by the salmon, displayed in the window title
	unsigned int points;

	// Game state
	float current_speed;
	
	// music references
	Mix_Music* background_music;

    bool has_battle_started;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
	MapState mapState;
	UnitMapState unitMapState;
    Level level = Level::sandbox;
    bool show_imgui = false;

    void imgui();
    void imgui_level_selection_menu();

    int imgui_entity_selection = 0;
    void imgui_sandbox_menu();

    TileType imgui_entity_selection_to_tileType();

    void imgui_save_level();

    void imgui_load_level();

    void load_grid(std::string);

    void imgui_tile_menu();

    void imgui_ally_menu();

    void imgui_enemy_menu();

    UnitType imgui_entity_selection_to_unitType();

    void init_unit_grid();

    void imgui_battle_control_menu();
};

