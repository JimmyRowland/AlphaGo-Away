// Header
#include "battle_screen_world.hpp"
#include "debug.hpp"
#include "physics.hpp"
#include "render_components.hpp"
#include "unit_factory.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>

// Game configuration
// TODO: Add any needed contant here
enum GRID_TYPE { BASIC, WATER, FOREST };
std::vector<std::vector<std::tuple<int, int>>> grid(10, std::vector<std::tuple<int, int>>(10) );

// Create the battle world
// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
BattleWorldSystem::BattleWorldSystem(ivec2 window_size_px, UnitFactory &unitFactory) :
        points(0), unitFactory(unitFactory) {
    // Seeding rng with random device
    rng = std::default_random_engine(std::random_device()());

    ///////////////////////////////////////
    // Initialize GLFW
    auto glfw_err_callback = [](int error, const char* desc) { std::cerr << "OpenGL:" << error << desc << std::endl; };
    glfwSetErrorCallback(glfw_err_callback);
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    //-------------------------------------------------------------------------
    // GLFW / OGL Initialization, needs to be set before glfwCreateWindow
    // Core Opengl 3.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, 0);

    // Create the main window (for rendering, keyboard, and mouse input)
    window = glfwCreateWindow(window_size_px.x, window_size_px.y, "Salmon Game Assignment", nullptr, nullptr);
    if (window == nullptr)
        throw std::runtime_error("Failed to glfwCreateWindow");

    // Setting callbacks to member functions (that's why the redirect is needed)
    // Input is handled using GLFW, for more info see
    // http://www.glfw.org/docs/latest/input_guide.html
    glfwSetWindowUserPointer(window, this);
    auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((BattleWorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
    auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((BattleWorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
    auto cursor_click_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) {((BattleWorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_click( _0, _1, _2); };
    glfwSetKeyCallback(window, key_redirect);
    glfwSetCursorPosCallback(window, cursor_pos_redirect);
    glfwSetMouseButtonCallback(window, cursor_click_redirect);
    // Playing background music indefinitely
    init_audio();
    Mix_PlayMusic(background_music, -1);
    std::cout << "Loaded music\n";
}

BattleWorldSystem::~BattleWorldSystem(){
    // Destroy music components
    if (background_music != nullptr)
        Mix_FreeMusic(background_music);
    if (salmon_dead_sound != nullptr)
        Mix_FreeChunk(salmon_dead_sound);
    if (salmon_eat_sound != nullptr)
        Mix_FreeChunk(salmon_eat_sound);
    Mix_CloseAudio();

    // Destroy all created components
    ECS::ContainerInterface::clear_all_components();

    // Close the window
    glfwDestroyWindow(window);
}

void BattleWorldSystem::init_audio()
{
    //////////////////////////////////////
    // Loading music and sounds with SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        throw std::runtime_error("Failed to initialize SDL Audio");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
        throw std::runtime_error("Failed to open audio device");

    background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
    salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav").c_str());
    salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav").c_str());

    if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr)
        throw std::runtime_error("Failed to load sounds make sure the data directory is present: "+
            audio_path("music.wav")+
            audio_path("salmon_dead.wav")+
            audio_path("salmon_eat.wav"));

}

void BattleWorldSystem::init_grid() {
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);
    int gridWidth = floor((winWidth - 20) / grid.size());
    int gridHeight = floor((winWidth - 20) / grid[0].size());
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            int xpos, ypos;
            if (i == 0) {
                xpos = 10 + gridWidth / 2;
            }
            else {
                xpos = std::get<0>(grid[i - 1][j]) + gridWidth;
            }
            if (j == 0) {
                ypos = 10 + gridHeight / 2;
            }
            else {
                ypos = std::get<1>(grid[i][j - 1]) + gridHeight;
            }
            grid[i][j] = std::tuple<int, int>{ xpos, ypos };
        }
    } 


    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            int xpos = std::get<0>(grid[i][j]);
            int ypos = std::get<1>(grid[i][j]);
            //std::cout << xpos << ", " << ypos << std::endl;
            ECS::Entity entity = Grid::createGrid({xpos, ypos}, GRID_TYPE::BASIC, "basic_grid.png", vec2(gridWidth, gridHeight));
        }
    }
    
    //ECS::Entity entity = Grid::createGrid({0.5, 0.5}, GRID_TYPE::BASIC, "basic_grid.png");
}

// Reset the world state to its initial state
void BattleWorldSystem::restart()
{
    
    // Debugging for memory/component leaks
    ECS::ContainerInterface::list_all_components();
    std::cout << "Restarting\n";


    // Reset the game speed
    current_speed = 1.f;

    // Remove all entities that we created
    // All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
    while (ECS::registry<Motion>.entities.size()>0)
        ECS::ContainerInterface::remove_all_components_of(ECS::registry<Motion>.entities.back());

    // Debugging for memory/component leaks
    ECS::ContainerInterface::list_all_components();

    init_grid();
    player_unit = unitFactory.create_unit({38, 30});


    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Add our grid map related entities.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
}

// Update our game world
void BattleWorldSystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
    // Updating window title with points
    std::stringstream title_ss;
    title_ss << "Points: " << points;
    glfwSetWindowTitle(window, title_ss.str().c_str());
    
    // Removing out of screen entities
    auto& registry = ECS::registry<Motion>;

    //create new unit

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: handle world update.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
}

// Compute collisions between entities
void BattleWorldSystem::handle_collisions()
{
    // Loop over all collisions detected by the physics system
    auto& registry = ECS::registry<PhysicsSystem::Collision>;
    for (unsigned int i=0; i< registry.components.size(); i++)
    {
        // The entity and its collider
//        auto entity = registry.entities[i];
//        auto entity_other = registry.components[i].other;

        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // TODO: handle collisions between units and enemies.
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }

    // Remove all collisions from this simulation step
    ECS::registry<PhysicsSystem::Collision>.clear();
}

// Should the game be over ?
bool BattleWorldSystem::is_over() const
{
    return glfwWindowShouldClose(window)>0;
}

// On key callback
// TODO A1: check out https://www.glfw.org/docs/3.3/input_guide.html
void BattleWorldSystem::on_key(int key, int, int action, int mod)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Add keyboard contorl here.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Resetting game
    if (action == GLFW_RELEASE && key == GLFW_KEY_R)
    {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        
        restart();
    }

    // Debugging
    if (key == GLFW_KEY_D)
        DebugSystem::in_debug_mode = (action != GLFW_RELEASE);

    // Control the current speed with `<` `>`
    if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA)
    {
        current_speed -= 0.1f;
        std::cout << "Current speed = " << current_speed << std::endl;
    }
    if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
    {
        current_speed += 0.1f;
        std::cout << "Current speed = " << current_speed << std::endl;
    }
    current_speed = std::max(0.f, current_speed);

    for(auto observer: keyBoardObservers){
        observer->on_key_click(key, action);
    }
}


void BattleWorldSystem::on_mouse_move(vec2 mouse_pos)
{

        (void)mouse_pos;
}

void BattleWorldSystem::on_mouse_click(int button, int action, int mods){
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        player_unit = unitFactory.create_unit({xpos, ypos});
    }
}

void BattleWorldSystem::on_collision(ECS::Entity entity_i, ECS::Entity entity_j) {
    Property& property_i = ECS::registry<Property>.get(entity_i);
    Property& property_j = ECS::registry<Property>.get(entity_j);

    if(property_j.isEnemy != property_i.isEnemy){
        property_i.hp -= property_j.damage;
        property_j.hp -= property_i.damage;
        if(property_i.hp <= 0){
            ECS::ContainerInterface::remove_all_components_of(entity_i);
            Mix_PlayChannel(-1, salmon_dead_sound, 0);
        }
        if(property_j.hp <= 0){
            ECS::ContainerInterface::remove_all_components_of(entity_j);
            Mix_PlayChannel(-1, salmon_dead_sound, 0);
        }
    }
}


