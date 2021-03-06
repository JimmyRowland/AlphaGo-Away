#pragma once

// internal
#include "common.hpp"
#include "map_grid.hpp"
#include "unit_factory.hpp"
#include "keyboard_observer.hpp"
#include "physics.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <utility>

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class BattleWorldSystem
{
public:
    // Creates a window
    BattleWorldSystem(ivec2 window_size_px, UnitFactory& unitFactory, std::tuple<float, int, int> grid_dim);

    // Releases all associated resources
    ~BattleWorldSystem();

    std::vector<KeyboardObserver *> keyBoardObservers;

    // restart level
    void restart();
    
    // Steps the game ahead by ms milliseconds
    void step(float elapsed_ms, vec2 window_size_in_game_units);

    // Check for collisions
    void handle_collisions();

    // observer function
    void on_collision(ECS::Entity entity_i, ECS::Entity entity_j);

    // Renders our scene
    void draw();

    PhysicsSystem* physicsSystem;


    // Should the game be over ?
    bool is_over() const;
    // OpenGL window handle
    GLFWwindow* window;
private:
    // Input callback functions
    void on_key(int key, int, int action, int mod);
    void on_mouse_move(vec2 mouse_pos);
    void on_mouse_click(int button, int action, int mods);

    // Loads the audio
    void init_audio();
    // Loads the grid
    void init_grid();

    UnitFactory &unitFactory;
    std::tuple<float, int, int> grid_dim;


    // Number of fish eaten by the salmon, displayed in the window title
    unsigned int points;

    // Game state
    float current_speed;
    ECS::Entity init_player_unit_0;
    ECS::Entity init_player_unit_1;
    ECS::Entity init_player_unit_2;
    ECS::Entity init_ai_2;
    ECS::Entity init_ai_1;
    ECS::Entity init_ai_3;
    ECS::Entity player_unit;

    
    // TODO: Add grids for later rendering.
    
    // music references
    Mix_Music* background_music;
    Mix_Chunk* salmon_dead_sound;
    Mix_Chunk* salmon_eat_sound;

    // C++ random number generator
    std::default_random_engine rng;
    std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
