// Header
#include "game.hpp"
#include "system/physics.hpp"
#include "logger/debug.hpp"
#include "system/render_components.hpp"
// stlib
#include <cassert>
#include <sstream>
#include <iostream>

// Game configuration
const size_t TURTLE_DELAY_MS = 2000;

// Create the fish world
// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
Game::Game(ivec2 window_size_px) :
	points(0),
	next_turtle_spawn(0.f)
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
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((Game*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((Game*)glfwGetWindowUserPointer(wnd))->on_mouse_move({_0, _1 }); };
    auto cursor_click_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2) {
        ((Game*) glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2);
    };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
    glfwSetMouseButtonCallback(window, cursor_click_redirect);

	// Playing background music indefinitely
	init_audio();
	Mix_PlayMusic(background_music, -1);
	std::cout << "Loaded music\n";
}

Game::~Game(){
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	Mix_CloseAudio();

	// Close the window
	glfwDestroyWindow(window);
}

void Game::init_audio()
{
	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
		throw std::runtime_error("Failed to initialize SDL Audio");

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
		throw std::runtime_error("Failed to open audio device");

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());

	if (background_music == nullptr)
		throw std::runtime_error("Failed to load sounds make sure the data directory is present: "+
			audio_path("music.wav"));

}

// Update our game world
void Game::update(float elapsed_ms, vec2 window_size_in_game_units)
{

	// Spawning new turtles
	next_turtle_spawn -= elapsed_ms * current_speed;
	if (next_turtle_spawn < 0.f)
	{
		// Reset timer
		next_turtle_spawn = (TURTLE_DELAY_MS / 2) + uniform_dist(rng) * (TURTLE_DELAY_MS / 2);
		// Create turtle
		entt::entity entity = ground_unit_factory({0,0});
		// Setting random initial position and constant velocity
		auto& motion = m_registry.get<Motion>(entity);
        auto& position = m_registry.get<Position>(entity);

        position.position = vec2(window_size_in_game_units.x - 150.f, 50.f + uniform_dist(rng) * (window_size_in_game_units.y - 100.f));
		motion.velocity = vec2(-100.f, 0.f );

	}

}

// Reset the world state to its initial state
void Game::restart()
{
	std::cout << "Restarting\n";
	current_speed = 1.f;



    init_level();
    init_grid();
    ground_unit_factory(vec2(80,80));

}

// Compute collisions between entities
void Game::handle_collisions()
{
    points;

}

// Should the game be over ?
bool Game::is_over() const
{
	return glfwWindowShouldClose(window)>0;
}


void Game::on_key(int key, int, int action, int mod)
{


	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		
//		restart();
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


void sandbox_on_key(int key, int, int action, int mod){

}

vec2 Game::get_cursor_position(){
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return vec2(xpos, ypos);
}

ivec2 Game::get_window_size(){
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);
    return ivec2(winWidth, winHeight);
}

void Game::on_mouse_click(int button, int action, int mods) {
    if(game_state.is_sandbox) return sandbox_on_click(button,action,mods);
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
//        double xpos, ypos;
//        glfwGetCursorPos(window, &xpos, &ypos);
//        int winWidth, winHeight;
//        glfwGetWindowSize(window, &winWidth, &winHeight);
//        auto gridWidth = (floor((winWidth - 20) / grid.size())) / 2;
//        auto gridHeight = (floor((winWidth - 20) / grid[0].size())) / 2;
//        //auto& selected_unit = ECS::registry<Unit>.entities[0];
//        if (physicsSystem->should_pause) {
//            if (action == GLFW_PRESS) {
//                for (auto entity : ECS::registry<Property>.entities) {
//                    auto &motion = ECS::registry<Motion>.get(entity);
//                    auto dis_x = abs(motion.position.x - xpos);
//                    auto dis_y = abs(motion.position.y - ypos);
//                    if (dis_x < gridWidth && dis_y < gridHeight) {
//                        //Propertyed_unit = entity;
//                        auto &property = ECS::registry<Property>.get(entity);
//                        property.selected = true;
//                        glfwGetCursorPos(window, &xpos, &ypos);
//                        motion.position.x = xpos;
//                        motion.position.y = ypos;
//                        break;
//                    }
//                }
//            } else if (action == GLFW_RELEASE) {
//                for (auto entity : ECS::registry<Unit>.entities) {
//                    auto &property = ECS::registry<Property>.get(entity);
//                    if (property.selected) {
//                        auto &motion = ECS::registry<Motion>.get(entity);
//                        int grid_pos_x = std::get<0>(grid[grid[0].size() - 1][0]);
//                        int grid_pos_y = std::get<1>(grid[0][grid[0].size() - 1]);
//                        glfwGetCursorPos(window, &xpos, &ypos);
//                        auto dis_x = xpos - grid_pos_x;
//                        auto dis_y = ypos - grid_pos_y;
//                        if (dis_x > gridWidth || dis_y > gridHeight) {
//
//                            property.selected = false;
//                        } else {
//                            for (int i = 0; i < grid[0].size(); i++) {
//                                for (int j = 0; j < grid[0].size(); j++) {
//                                    grid_pos_x = std::get<0>(grid[i][j]);
//                                    grid_pos_y = std::get<1>(grid[i][j]);
//                                    glfwGetCursorPos(window, &xpos, &ypos);
//                                    dis_x = abs(xpos - grid_pos_x);
//                                    dis_y = abs(ypos - grid_pos_y);
//                                    if (dis_x < gridWidth && dis_y < gridHeight) {
//                                        motion.position.x = grid_pos_x;
//                                        motion.position.y = grid_pos_y;
//                                        property.selected = false;
//                                        break;
//                                    }
//                                }
//                            }
//                        }
//
//
//                    }
//                }
//
//
//            }
//        } else {
//            if (action==GLFW_PRESS && xpos > 30.f && xpos < 570.f && ypos > 30.f && ypos < 570.f) {
//                unitFactory.create_unit({xpos, ypos});
//            }
//        }
    }
}

void Game::sandbox_on_click(int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        auto cursor_position = get_cursor_position();
        auto entity = get_tile_entity_at_position(cursor_position);
        swap_tile_texture(entity, TileType::basic);
    }
}

void Game::on_mouse_move(vec2 mouse_pos)
{

		(void)mouse_pos;

}

void Game::init_level() {
    mapState = makeMapState();
}


void Game::init_grid() {
    for (int i = 0; i < tile_matrix_dimension.x; i++) {
        float xpos = tile_size.x/2 + tile_size.x * i;
        for (int j = 0; j < tile_matrix_dimension.y; j++) {
            float ypos = tile_size.y/2 + tile_size.y * j;
            tile_factory(vec2(xpos,ypos), mapState[ivec2(i,j)]);
        }
    }
}
