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
enum GRID_TYPE {
    BASIC, WATER, FOREST
};
std::vector<std::vector<std::tuple<int, int>>> grid(10, std::vector<std::tuple<int, int>>(10));

// Create the battle world
// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
BattleWorldSystem::BattleWorldSystem(ivec2 window_size_px, UnitFactory &unitFactory, std::tuple<float, int, int> grid_dim) :
        points(0), unitFactory(unitFactory) {
    // Seeding rng with random device
    rng = std::default_random_engine(std::random_device()());

    ///////////////////////////////////////
    // Initialize GLFW
    auto glfw_err_callback = [](int error, const char *desc) { std::cerr << "OpenGL:" << error << desc << std::endl; };
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
    auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2,
                           int _3) { ((BattleWorldSystem *) glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
    auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1) {
        ((BattleWorldSystem *) glfwGetWindowUserPointer(wnd))->on_mouse_move({_0, _1});
    };
    auto cursor_click_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2) {
        ((BattleWorldSystem *) glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2);
    };
    glfwSetKeyCallback(window, key_redirect);
    glfwSetCursorPosCallback(window, cursor_pos_redirect);
    glfwSetMouseButtonCallback(window, cursor_click_redirect);
    // Playing background music indefinitely
    init_audio();
    Mix_PlayMusic(background_music, -1);
    std::cout << "Loaded music\n";
    this->grid_dim = grid_dim;
}

BattleWorldSystem::~BattleWorldSystem() {
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

void BattleWorldSystem::init_audio() {
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
        throw std::runtime_error("Failed to load sounds make sure the data directory is present: " +
                                 audio_path("music.wav") +
                                 audio_path("salmon_dead.wav") +
                                 audio_path("salmon_eat.wav"));

}

void BattleWorldSystem::init_grid() {
    grid.resize(std::get<1>(this->grid_dim));
    for (int i = 0; i < grid.size(); i++) {
        grid[i].resize(std::get<2>(this->grid_dim));
    }
    int winWidth, winHeight;    
    glfwGetWindowSize(window, &winWidth, &winHeight);
    int gridWidth = floor(std::get<0>(this->grid_dim) / std::get<1>(this->grid_dim));
    int gridHeight = floor(std::get<0>(this->grid_dim) / std::get<1>(this->grid_dim));
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            int xpos, ypos;
            if (i == 0) {
                xpos = (winWidth - std::get<0>(this->grid_dim)) / 2 + gridWidth / 2;
            } else {
                xpos = std::get<0>(grid[i - 1][j]) + gridWidth;
            }
            if (j == 0) {
                ypos = (winHeight - std::get<0>(this->grid_dim)) / 2 + gridHeight / 2;
            } else {
                ypos = std::get<1>(grid[i][j - 1]) + gridHeight;
            }
            grid[i][j] = std::tuple<int, int>{xpos, ypos};
        }
    }


    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            int xpos = std::get<0>(grid[i][j]);
            int ypos = std::get<1>(grid[i][j]);
            //std::cout << xpos << ", " << ypos << std::endl;
            ECS::Entity entity = Grid::createGrid({xpos, ypos}, GRID_TYPE::BASIC, "basic_grid.png",
                                                  vec2(gridWidth, gridHeight), std::make_pair(i, j));
        }
    }
    grid_initialized = true;

    //ECS::Entity entity = Grid::createGrid({0.5, 0.5}, GRID_TYPE::BASIC, "basic_grid.png");
}

// Reset the world state to its initial state
void BattleWorldSystem::restart() {

    // Debugging for memory/component leaks
    ECS::ContainerInterface::list_all_components();
    std::cout << "Restarting\n";


    // Reset the game speed
    current_speed = 1.f;

    // Remove all entities that we created
    // All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
    while (ECS::registry<Motion>.entities.size() > 0)
        ECS::ContainerInterface::remove_all_components_of(ECS::registry<Motion>.entities.back());

    // Debugging for memory/component leaks
    ECS::ContainerInterface::list_all_components();
    
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);
    
    if (state == 0)
    {
        // rendering of the start screen
        background = ScreenComponent::createScreenComponent({winWidth/2, winHeight/2}, "BgScreens/frame_0_delay-0.11s.png", {winWidth, winHeight}, 10000002, COMPONENT_TYPE::BASICBG);
        game_title = ScreenComponent::createScreenComponent({winWidth/2, winHeight/2 - 160}, "gameTitle.png", {360, 200}, 998, COMPONENT_TYPE::BASICBG);
        button_play = ScreenComponent::createScreenComponent({winWidth/2, winHeight/2 + 70}, "buttons/PlayButton.jpg", {150, 70}, 997, COMPONENT_TYPE::BUTTON);
        button_help = ScreenComponent::createScreenComponent({winWidth/2, winHeight/2 + 160}, "buttons/HelpButton.jpg", {150, 70}, 996, COMPONENT_TYPE::BASICBG);
        button_quit = ScreenComponent::createScreenComponent({winWidth/2, winHeight/2 + 250}, "buttons/QuitButton.jpg", {150, 70}, 995, COMPONENT_TYPE::BASICBG);
    }


	//init_player_unit_0 = unitFactory.create_unit({ 38, 30 }, Terminator, {0, 0});
    

}

// Update our game world
void BattleWorldSystem::step(float elapsed_ms, vec2 window_size_in_game_units) {
    int winWidth = window_size_in_game_units.x;
    int winHeight = window_size_in_game_units.y;
    if (state == 0) {
        frame += 0.11;
        int bg_num = ((int) floor(frame)) % 32;
        //    std::cout << "current frame number: " << bg_num << std::endl;
        //    ECS::ContainerInterface::remove_all_components_of(background);
        // Update the frame of background
        background = ScreenComponent::createScreenComponent({winWidth/2, winHeight/2}, "BgScreens/frame_" + std::to_string(bg_num) + "_delay-0.11s.png", {winWidth, winHeight}, 10000000 - (frame*0.1), COMPONENT_TYPE::BASICBG);
        grid_initialized = false;
        
    } else if (state == 1) {
        battle_frame += 0.2;
        int bg_num = ((int) floor(battle_frame)) % 12;
        ECS::ContainerInterface::remove_all_components_of(background);
        ECS::ContainerInterface::remove_all_components_of(game_title);
        ECS::ContainerInterface::remove_all_components_of(button_play);
        ECS::ContainerInterface::remove_all_components_of(button_help);
        ECS::ContainerInterface::remove_all_components_of(button_quit);
        battle_background = ScreenComponent::createScreenComponent({winWidth/2, winHeight/2}, "BattleScreens/frame_" + std::to_string(bg_num) + ".png", {winWidth, winHeight}, 1000000 - (battle_frame*0.001), COMPONENT_TYPE::BASICBG);
        
        if (!grid_initialized) {
            init_grid();
            int winWidth, winHeight;
            glfwGetWindowSize(window, &winWidth, &winHeight);
            int gridWidth = floor((winWidth - 20) / grid.size());
            int gridHeight = floor((winWidth - 20) / grid[0].size());
            unitFactory.setGridHeight(gridHeight);
            unitFactory.setGridWidth(gridWidth);

            //init_player_unit_1 = unitFactory.create_unit({ 38, 30 + gridHeight }, H_Archer, {0, 1});
            //init_player_unit_2 = unitFactory.create_unit({ 38, 30 + gridHeight * 2 }, H_Terminator, {0, 2});
            //init_ai_1 = unitFactory.create_unit({ 38 + 9 * gridWidth, 30 + 4 * gridHeight }, H_Monitor, {10, 5});
            //init_ai_2 = unitFactory.create_unit({ 38 + 9 * gridWidth, 30 + 5 * gridHeight }, H_Healer, {10,6});
            //init_ai_3 = unitFactory.create_unit({ 38 + 9 * gridWidth, 30 + 6 * gridHeight }, H_Monitor, {10, 7});
            //init_ai_2 = unitFactory.create_unit({ 38 + 9 * gridWidth, 30 + 4 * gridHeight }, A_Archer, { 10,5 });
//            init_ai_3 = unitFactory.create_unit({ 38 + 9 * gridWidth, 30 + 5 * gridHeight }, H_Archer, { 10,5 });
        }
    }

    
    
    // Updating window title with points
    std::stringstream title_ss;
    title_ss << "Points: " << points << " Gold: " << unitFactory.getLevelState().getGold() << " Health: " << unitFactory.getLevelState().getHealthTotal();
    glfwSetWindowTitle(window, title_ss.str().c_str());
    int gridWidth = floor((window_size_in_game_units.x - 20) / grid.size());
    int gridHeight = floor((window_size_in_game_units.y - 20) / grid[0].size());
    float boardWidth = 9 * gridWidth + 38;
    float boardHeight = 9 * gridHeight - 150;
    for (auto entity: ECS::view<Motion, Property>()) {
        Motion &motion = ECS::registry<Motion>.get(entity);
        if (motion.position.y < 30.f) {
            motion.position.y = 30.f;
            if (motion.velocity.y < 0) {
                motion.velocity.y = 0;
            }
        } else if (motion.position.y > boardHeight) {
            motion.position.y = boardHeight;
            if (motion.velocity.y > 0) {
                motion.velocity.y = 0;
            }
        }
        if (motion.position.x - 30.f < 0) {
            if (motion.velocity.x < 0) {
                motion.velocity.x = 0;
            }
            motion.position.x = 30.f;
        } else if (motion.position.x > boardWidth) {
            if (motion.velocity.x > 0) {
                motion.velocity.x = 0;
            }
            motion.position.x = boardWidth;
        }
        if (ECS::registry<BoundingBox>.has(entity)) {
            auto& bb = ECS::registry<BoundingBox>.get(entity);
            Transform transform;
            transform.translate(motion.position);
            transform.rotate(motion.angle);
            transform.scale(motion.scale);
            std::vector<vec2> temp_vector;
            for (int i = 0; i < bb.vertices.size(); i++) {
                auto transformed = transform.mat * vec3(bb.vertices[i].x, bb.vertices[i].y, 1.f);
                temp_vector.push_back(vec2(transformed.x, transformed.y));
                bb.transformed_vertices = temp_vector;
                //std::cout << bb.transformed_vertices[i].x << "," << bb.transformed_vertices[i].y << "," << motion.position.x << "," << motion.position.y << std::endl;
            }
        }
    }
    

}

// Compute collisions between entities
void BattleWorldSystem::handle_collisions() {
    // Loop over all collisions detected by the physics system
    auto &registry = ECS::registry<PhysicsSystem::Collision>;
    for (unsigned int i = 0; i < registry.components.size(); i++) {
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
bool BattleWorldSystem::is_over() const {
    return glfwWindowShouldClose(window) > 0;
}

// On key callback

void BattleWorldSystem::on_key(int key, int, int action, int mod) {
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Add keyboard contorl here.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // Resetting game
    if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
        int w, h;
        glfwGetWindowSize(window, &w, &h);

        restart();
    }

    // Debugging
    if (key == GLFW_KEY_B)
        DebugSystem::in_debug_mode = (action != GLFW_RELEASE);

    // Control the current speed with `<` `>`
    if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
        current_speed -= 0.1f;
        std::cout << "Current speed = " << current_speed << std::endl;
    }
    if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
        current_speed += 0.1f;
        std::cout << "Current speed = " << current_speed << std::endl;
    }
    current_speed = std::max(0.f, current_speed);

    for (auto observer: keyBoardObservers) {
        observer->on_key_click(key, action);
    }
}


void BattleWorldSystem::on_mouse_move(vec2 mouse_pos) {

    (void) mouse_pos;
}

void BattleWorldSystem::on_mouse_click(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int winWidth, winHeight;
        glfwGetWindowSize(window, &winWidth, &winHeight);
        auto gridWidth = (floor((winWidth - 20) / grid.size())) / 2;
        auto gridHeight = (floor((winWidth - 20) / grid[0].size())) / 2;
        //auto& selected_unit = ECS::registry<Unit>.entities[0];
        if (physicsSystem->should_pause) {
            if (action == GLFW_PRESS) {
                for (auto entity : ECS::registry<Property>.entities) {
                    auto &motion = ECS::registry<Motion>.get(entity);
                    auto dis_x = abs(motion.position.x - xpos);
                    auto dis_y = abs(motion.position.y - ypos);
                    if (dis_x < gridWidth && dis_y < gridHeight) {
                        //Propertyed_unit = entity;
                        auto &property = ECS::registry<Property>.get(entity);
                        property.selected = true;
                        glfwGetCursorPos(window, &xpos, &ypos);
                        motion.position.x = xpos;
                        motion.position.y = ypos;
                        break;
                    }
                }
            } else if (action == GLFW_RELEASE) {
                for (auto entity : ECS::registry<Unit>.entities) {
                    auto &property = ECS::registry<Property>.get(entity);
                    if (property.selected) {
                        auto &motion = ECS::registry<Motion>.get(entity);
                        int grid_pos_x = std::get<0>(grid[grid[0].size() - 1][0]);
                        int grid_pos_y = std::get<1>(grid[0][grid[0].size() - 1]);
                        glfwGetCursorPos(window, &xpos, &ypos);
                        auto dis_x = xpos - grid_pos_x;
                        auto dis_y = ypos - grid_pos_y;
                        if (dis_x > gridWidth || dis_y > gridHeight) {

                            property.selected = false;
                        } else {
                            for (int i = 0; i < grid[0].size(); i++) {
                                for (int j = 0; j < grid[0].size(); j++) {
                                    grid_pos_x = std::get<0>(grid[i][j]);
                                    grid_pos_y = std::get<1>(grid[i][j]);
                                    glfwGetCursorPos(window, &xpos, &ypos);
                                    dis_x = abs(xpos - grid_pos_x);
                                    dis_y = abs(ypos - grid_pos_y);
                                    if (dis_x < gridWidth && dis_y < gridHeight) {
                                        motion.position.x = grid_pos_x;
                                        motion.position.y = grid_pos_y;
                                        property.selected = false;
                                        break;
                                    }
                                }
                            }
                        }


                    }
                }


            }
        } else {
            if (action==GLFW_PRESS && xpos > 30.f && xpos < 570.f && ypos > 30.f && ypos < 570.f) {
                unitFactory.create_unit({xpos, ypos}, UnitFactory::curType, vec2((xpos - 30)/30, (ypos - 30)/30));
            }
        }
    }
    
    // check if any button is clicked
    if ((button == GLFW_MOUSE_BUTTON_LEFT) && (state == 0)) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        auto& play_button = ECS::registry<Motion>.get(button_play);
        vec2 play_position = play_button.position;
        vec2 play_scale = play_button.scale;
        bool x_in_button = (xpos <= (play_position.x + play_scale.x)) && (xpos >= (play_position.x - play_scale.x));
        bool y_in_button = (ypos <= (play_position.y + play_scale.y)) && (xpos >= (play_position.y - play_scale.y));
        if (x_in_button && y_in_button) {
            state = 1;
        }
        
    }
}

void BattleWorldSystem::on_collision(ECS::Entity entity_i, ECS::Entity entity_j) {
    Property &property_i = ECS::registry<Property>.get(entity_i);
    Property &property_j = ECS::registry<Property>.get(entity_j);

    if (property_j.isEnemy != property_i.isEnemy) {
        property_i.hp -= property_j.damage;
        property_j.hp -= property_i.damage;
        if (property_i.hp <= 0) {
            ECS::ContainerInterface::remove_all_components_of(entity_i);
            Mix_PlayChannel(-1, salmon_dead_sound, 0);
        }
        if (property_j.hp <= 0) {
            ECS::ContainerInterface::remove_all_components_of(entity_j);
            Mix_PlayChannel(-1, salmon_dead_sound, 0);
        }
    }
}


