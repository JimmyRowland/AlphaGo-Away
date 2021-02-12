// Header
#include "battle_screen_world.hpp"
#include "debug.hpp"
#include "physics.hpp"
#include "render_components.hpp"
#include "unit.hpp"
#include "enemy.hpp"

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
BattleWorldSystem::BattleWorldSystem(ivec2 window_size_px) :
    points(0)
{
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
    auto mouse_button_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) {
        ((BattleWorldSystem *) glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
    glfwSetKeyCallback(window, key_redirect);
    // get cursor position

    //glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    glfwSetMouseButtonCallback(window,mouse_button_redirect);
    //glfwSetCursorPosCallback(window, cursor_pos_redirect);

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
    init_player_unit_0 = Unit::createUnit({38, 30});
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);
    int gridWidth = floor((winWidth - 20) / grid.size());
    int gridHeight = floor((winWidth - 20) / grid[0].size());
    init_player_unit_1 = Unit::createUnit({38, 30+gridHeight});
    init_player_unit_2 = Unit::createUnit({38, 30+gridHeight*2});
    init_ai_1 = Enemy::createEnemy({38+9*gridWidth, 30+4*gridHeight}, "ai_default.png", vec2(gridWidth, gridHeight));
    init_ai_2 = Enemy::createEnemy({38+9*gridWidth, 30+5*gridHeight}, "ai_default.png", vec2(gridWidth, gridHeight));
    init_ai_3 = Enemy::createEnemy({38+9*gridWidth, 30+6*gridHeight}, "ai_default.png", vec2(gridWidth, gridHeight));

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

void BattleWorldSystem::on_key(int key, int, int action, int mod)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Add keyboard contorl here.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // For M1: when press B, both AI and unit simply walk forward.
    if (key == GLFW_KEY_B && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        for(auto entity : ECS::registry<Unit>.entities){
            auto& motion = ECS::registry<Motion>.get(entity);
            motion.trigger = 1;
            //motion.velocity = {50.f, 0.f};
        }
        for(auto entity : ECS::registry<Enemy>.entities){
            auto& motion = ECS::registry<Motion>.get(entity);
            motion.trigger = 1;
            //motion.velocity = {-50.f, 0.f};
        }

    }

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
}


void BattleWorldSystem::on_mouse_click(int button, int action, int mods)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Add mouse contorl here.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

      //When keep mouse left key pressed,
      // you can drag to arrange unit's location and one map grid only contain one unit
      //When you release the key, the unit will fall into the grid where the cursor is currently located.
      // If the grid already contain one unit, the unit will automatically fall into the adjacent one or back to the original place (wait for implement)
      //For current M1, the unit can be placed anywhere on the map, when our explicit battle mechanism come out, will limit the unit's position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);
    auto gridWidth = (floor((winWidth - 20) / grid.size()))/2;
    auto gridHeight = (floor((winWidth - 20) / grid[0].size()))/2;
    //auto& selected_unit = ECS::registry<Unit>.entities[0];
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        for(auto entity : ECS::registry<Unit>.entities){
            auto& motion = ECS::registry<Motion>.get(entity);
            auto dis_x = abs(motion.position.x - xpos);
            auto dis_y = abs(motion.position.y - ypos);
            if(dis_x<gridWidth && dis_y<gridHeight){
                //selected_unit = entity;
                auto& selection = ECS::registry<Select>.get(entity);
                selection.select = 1;
                selection.origin_pos_x = motion.position.x;
                selection.origin_pos_y = motion.position.y;
                glfwGetCursorPos(window, &xpos, &ypos);
                motion.position.x = xpos;
                motion.position.y = ypos;
                break;
            }
        }
    }else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        for(auto entity : ECS::registry<Unit>.entities){
            auto& selection = ECS::registry<Select>.get(entity);
            if(selection.select){
                auto& motion = ECS::registry<Motion>.get(entity);
                int grid_pos_x = std::get<0>(grid[grid[0].size()-1][0]);
                int grid_pos_y = std::get<1>(grid[0][grid[0].size()-1]);
                glfwGetCursorPos(window, &xpos, &ypos);
                auto dis_x = xpos - grid_pos_x;
                auto dis_y = ypos - grid_pos_y;
                if(dis_x > gridWidth || dis_y> gridHeight){
                    motion.position.x = selection.origin_pos_x;
                    motion.position.y =selection.origin_pos_y;
                    selection.select = 0;
                } else {
                    for (int i = 0; i < grid[0].size(); i++) {
                        for (int j = 0; j < grid[0].size(); j++) {
                             grid_pos_x = std::get<0>(grid[i][j]);
                             grid_pos_y = std::get<1>(grid[i][j]);
                            glfwGetCursorPos(window, &xpos, &ypos);
                            dis_x = abs(xpos - grid_pos_x);
                            dis_y = abs(ypos - grid_pos_y);
                            if( dis_x<gridWidth && dis_y<gridHeight ){
                                motion.position.x = grid_pos_x;
                                motion.position.y = grid_pos_y;
                                selection.select = 0;
                                break;
                            }
                        }
                    }
                }


            }
        }


    }


    }

