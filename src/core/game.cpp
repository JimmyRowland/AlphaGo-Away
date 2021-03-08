// Header
#include "game.hpp"
// stlib
#include <cassert>
#include <sstream>



// Game configuration
const size_t TURTLE_DELAY_MS = 2000;

// Create the fish world
// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
Game::Game(ivec2 window_size_px) :
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
	window = glfwCreateWindow(window_size_px.x, window_size_px.y, "AlphaGo-Away", nullptr, nullptr);
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
//	Mix_PlayMusic(background_music, -1);
	std::cout << "Loaded music\n";
}

Game::~Game(){
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	Mix_CloseAudio();

	// Close the window
	glfwDestroyWindow(window);
    glfwTerminate();
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

    imgui();

}

// Reset the world state to its initial state
void Game::restart()
{
	std::cout << "Restarting\n";
	current_speed = 1.f;
    has_battle_started = false;
    init_level();
    init_map_grid();
    init_unit_grid();
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
	if (action == GLFW_RELEASE && key == GLFW_KEY_D){
        DebugSystem::in_debug_mode = !DebugSystem::in_debug_mode;
        show_imgui = DebugSystem::in_debug_mode;
	}

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
    if(level == Level::sandbox) return sandbox_on_click(button, action, mods);
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

TileType Game::imgui_entity_selection_to_tileType(){
    switch(imgui_entity_selection){
        case 1: return TileType::basic;
        case 2: return TileType::water;
        case 3: return TileType::forest;
        default: assert(false);
    }
}

UnitType Game::imgui_entity_selection_to_unitType(){
    switch(imgui_entity_selection){
        case 4: return UnitType::human_terminator;
        case 5: return UnitType::human_monitor;
        case 6: return UnitType::human_archer;
        case 7: return UnitType::human_healer;
        case 8: return UnitType::ai_terminator;
        case 9: return UnitType::ai_monitor;
        case 10: return UnitType::ai_archer;
        case 11: return UnitType::ai_healer;
        default: assert(false);
    }
}

void Game::sandbox_on_click(int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if(imgui_entity_selection > 0){
            auto cursor_position = get_cursor_position();
            ivec2 tile_index = get_tile_index(cursor_position);
            if(!is_tile_out_of_index(tile_index)){
                if(imgui_entity_selection < 4 && unitMapState[tile_index]==UnitType::empty){
                    auto entity = get_tile_entity_at_position(cursor_position);
                    swap_tile_texture(entity, imgui_entity_selection_to_tileType());
                    mapState[ivec2(tile_index.x, tile_index.y)] = imgui_entity_selection_to_tileType();
                }else if(imgui_entity_selection < 12 && imgui_entity_selection>3 && mapState[tile_index]==TileType::basic && unitMapState[tile_index]==UnitType::empty){
                    unitMapState[tile_index]=imgui_entity_selection_to_unitType();
                    unit_factory(get_tile_center_from_index(tile_index), imgui_entity_selection_to_unitType());
                }
            }
        }
    }
}

void Game::on_mouse_move(vec2 mouse_pos)
{
		(void)mouse_pos;
}

void Game::init_level() {
    mapState = makeMapState();
    unitMapState = makeUnitState();
}


void Game::init_map_grid() {
    for (int i = 0; i < tile_matrix_dimension.x; i++) {
        float xpos = tile_size.x/2 + tile_size.x * i;
        for (int j = 0; j < tile_matrix_dimension.y; j++) {
            float ypos = tile_size.y/2 + tile_size.y * j;
            tile_factory(vec2(xpos,ypos), mapState[ivec2(i,j)]);
        }
    }
}
void Game::init_unit_grid() {
    for (int i = 0; i < tile_matrix_dimension.x; i++) {
        float xpos = tile_size.x/2 + tile_size.x * i;
        for (int j = 0; j < tile_matrix_dimension.y; j++) {
            float ypos = tile_size.y/2 + tile_size.y * j;
            if(unitMapState[ivec2(i,j)]!=UnitType::empty){
                unit_factory(vec2(xpos,ypos), unitMapState[ivec2(i,j)]);
            }
        }
    }
}

namespace {
    void imgui_help_menu(){
        if (ImGui::CollapsingHeader("Help"))
        {
            ImGui::Text("ABOUT THIS DEMO:");
            ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
            ImGui::BulletText("The \"Examples\" menu above leads to more demo contents.");
            ImGui::BulletText("The \"Tools\" menu above gives access to: About Box, Style Editor,\n"
                              "and Metrics/Debugger (general purpose Dear ImGui debugging tool).");
            ImGui::Separator();

            ImGui::Text("PROGRAMMER GUIDE:");
            ImGui::BulletText("See the ShowDemoWindow() code in imgui_demo.cpp. <- you are here!");
            ImGui::BulletText("See comments in imgui.cpp.");
            ImGui::BulletText("See example applications in the examples/ folder.");
            ImGui::BulletText("Read the FAQ at http://www.dearimgui.org/faq/");
            ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
            ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
            ImGui::Separator();

            ImGui::Text("USER GUIDE:");
            ImGui::ShowUserGuide();
        }
    }
}

void Game::imgui_level_selection_menu(){
    if (ImGui::CollapsingHeader("Select a level"))
    {
        if (ImGui::Button("Sandbox")) level = Level::sandbox;
        if (ImGui::Button("level1")) level = Level::level1;
        if (ImGui::Button("level2")) level = Level::level2;
        if (ImGui::Button("level3")) level = Level::level3;
        if (ImGui::Button("level4")) level = Level::level4;
        if (ImGui::Button("level5")) level = Level::level5;
        if (ImGui::Button("Start Screen")) level = Level::start_screen;
    }
};

void Game::imgui_battle_control_menu(){
    if (ImGui::CollapsingHeader("Battle"))
    {
        if (ImGui::Button("Start battle")) has_battle_started = true;
        if (ImGui::Button("Restart level")) restart();
    }
};

void Game::imgui_save_level(){
    std::string map;
    for(int j = 0; j < tile_matrix_dimension.y; j++){
        for(int i = 0; i< tile_matrix_dimension.x; i++){
            map += tileType_to_char(mapState[ivec2(i,j)]);
        }
    }
    nlohmann::json json;
    json["map"] = map;
    std::ofstream o("sandbox.json");
    o << std::setw(4) << json << std::endl;
}

void Game::load_grid(std::string map_string) {
    auto entities = m_registry.view<Tile>();
    for (int i = 0; i < tile_matrix_dimension.x; i++) {
        for (int j = 0; j < tile_matrix_dimension.y; j++) {
            int index = i + j * tile_matrix_dimension.x;
            TileType tileType =  char_to_tileType(map_string[index]);
            mapState[ivec2(i,j)] = tileType;
            auto entity = entities[get_entity_index_from_tile_index(i,j)];
            swap_tile_texture(entity, tileType);
        }
    }
}

void Game::imgui_load_level(){
    std::ifstream i("sandbox.json");
    nlohmann::json json;
    i >> json;
    load_grid(json["map"]);
}

void Game::imgui_sandbox_menu(){
    if (level == Level::sandbox && ImGui::CollapsingHeader("sandbox"))
    {
        if (ImGui::Button("Save Level")) imgui_save_level();
        if (ImGui::Button("Load Level")) imgui_load_level();
        imgui_tile_menu();
        imgui_enemy_menu();
    }
};

void Game::imgui_tile_menu(){
    if (ImGui::CollapsingHeader("Tiles"))
    {
        ImGui::Text("Choose a tile type and click on map to change tiles");
        ImGui::RadioButton("disabled", &imgui_entity_selection, 0);
        ImGui::RadioButton("basic", &imgui_entity_selection, 1);
        ImGuiImage(get_tile_texture_id(TileType::basic));
        ImGui::RadioButton("water", &imgui_entity_selection, 2);
        ImGuiImage(get_tile_texture_id(TileType::water));
        ImGui::RadioButton("forest", &imgui_entity_selection, 3);
        ImGuiImage(get_tile_texture_id(TileType::forest));
    }
}

void Game::imgui_ally_menu(){
    if (ImGui::CollapsingHeader("Ally"))
    {
        ImGui::Text("Choose an ally type and click on map to place the unit");
        ImGui::RadioButton("disabled", &imgui_entity_selection, 0);
        ImGui::RadioButton("terminator", &imgui_entity_selection, 4);
//        ImGuiImage(get_tile_texture_id(TileType::basic));
        ImGui::RadioButton("monitor", &imgui_entity_selection, 5);
//        ImGuiImage(get_tile_texture_id(TileType::water));
        ImGui::RadioButton("archer", &imgui_entity_selection, 6);
//        ImGuiImage(get_tile_texture_id(TileType::forest));
        ImGui::RadioButton("healer", &imgui_entity_selection, 7);
//        ImGuiImage(get_tile_texture_id(TileType::forest));
    }
}

void Game::imgui_enemy_menu(){
    if (ImGui::CollapsingHeader("Enemy"))
    {
        ImGui::Text("Choose an enemy type and click on map to place the unit");
        ImGui::RadioButton("disabled", &imgui_entity_selection, 0);
        ImGui::RadioButton("terminator", &imgui_entity_selection, 8);
//        ImGuiImage(get_tile_texture_id(TileType::basic));
        ImGui::RadioButton("monitor", &imgui_entity_selection, 9);
//        ImGuiImage(get_tile_texture_id(TileType::water));
        ImGui::RadioButton("archer", &imgui_entity_selection, 10);
//        ImGuiImage(get_tile_texture_id(TileType::forest));
        ImGui::RadioButton("healer", &imgui_entity_selection, 11);
//        ImGuiImage(get_tile_texture_id(TileType::forest));
    }
}

void Game::imgui(){
    if(show_imgui){
        ImGui::Begin("Menu");
        imgui_battle_control_menu();
        imgui_help_menu();
        imgui_level_selection_menu();
        imgui_ally_menu();
        imgui_sandbox_menu();
        ImGui::End();
    }
}

