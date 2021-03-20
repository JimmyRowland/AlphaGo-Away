// Header
#include "game.hpp"




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
    if (this->level == Level::start_screen) {
        frame += 0.3;
        screenUpdate(frame);
    }
    if(has_battle_started){
        ai_update(elapsed_ms);
        physics_update(elapsed_ms);
    }
    update_camera_pos();
    imgui();

}

// Reset the world state to its initial state
void Game::restart(Level level)
{
    this->level = level;

    for(auto entity : m_registry.view<ShadedMeshRef>()){
        m_registry.destroy(entity);
    }
    std::cout << "Restarting\n";
    current_speed = 1.f;
    has_battle_started = false;

    if(level == Level::start_screen){
        frame = 1.f;
        loading_screen_factory();
        this->level = level;
    }else{
        background_factory();
        init_level();
        init_map_grid();
        init_unit_grid();
    }
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

void Game::update_camera_pos() {
    if(level!=Level::start_screen){
        ivec2 window_size = get_window_size();
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (abs(xpos - window_size.x) <= 20 || xpos <=20) {
            parallax_offset += 1 * xpos <= 20? 1 : -1;
            parallax_offset = ((int)parallax_offset % (int)(window_size.x));
            if (current_speed == 1) {
                for (auto&&[entity, position]: m_registry.view<Position,ScreenComponent>().each()){
                    position.position.x = window_size_in_game_units.x / 2 + parallax_offset;
                }
            }
        }
    }
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

//    game menu toggle
	if (action == GLFW_RELEASE && key == GLFW_KEY_D){
        show_imgui = !show_imgui;
	}

    // Debugging
    if (key == GLFW_KEY_B)
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
    if(level == Level::sandbox) return sandbox_on_click(button, action, mods);
    if(level == Level::level1 || level == Level::level2 ||level == Level::level3 ||level == Level::level4 ||level == Level::level5){
        return level_on_click(button, action, mods);
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
//        TODO restore relocating units
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

void Game::place_an_ally(ivec2 tile_index){
    if(imgui_entity_selection < 8 && imgui_entity_selection>3 && mapState[tile_index]==TileType::basic && unitMapState[tile_index]==UnitType::empty){
        unitMapState[tile_index]=imgui_entity_selection_to_unitType();
        unit_factory(get_tile_center_from_index(tile_index), imgui_entity_selection_to_unitType());
    }
}

void Game::place_an_enemy(ivec2 tile_index){
    if(imgui_entity_selection < 12 && imgui_entity_selection>7 && mapState[tile_index]==TileType::basic && unitMapState[tile_index]==UnitType::empty){
        unitMapState[tile_index]=imgui_entity_selection_to_unitType();
        unit_factory(get_tile_center_from_index(tile_index), imgui_entity_selection_to_unitType());
    }
}

void Game::sandbox_on_click(int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if(!has_battle_started){
            if(imgui_entity_selection > 0){
                auto cursor_position = get_cursor_position();
                ivec2 tile_index = get_tile_index(cursor_position);
                if(!is_tile_out_of_index(tile_index)){
//                TODO refactor sandbox_swap_tile
                    if(imgui_entity_selection < 4 && unitMapState[tile_index]==UnitType::empty){
                        auto entity = get_tile_entity_at_position(cursor_position);
                        swap_tile_texture(entity, imgui_entity_selection_to_tileType());
                        mapState[ivec2(tile_index.x, tile_index.y)] = imgui_entity_selection_to_tileType();
                        //                TODO refactor  sandbox_add_unit
                    }else{
                        place_an_ally(tile_index);
                        particles->emitParticle(get_tile_center_from_index(tile_index), 20);
                        place_an_enemy(tile_index);
                        particles->emitParticle(get_tile_center_from_index(tile_index), 20);
                    }
                }
            }
        }
    }
}

void Game::level_on_click(int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if(!has_battle_started){
            std::cout << imgui_entity_selection << '\n';
            if(imgui_entity_selection < 8 && imgui_entity_selection>3){
                auto cursor_position = get_cursor_position();
                ivec2 tile_index = get_tile_index(cursor_position);
                std::cout << "tile_index" << tile_index.x << tile_index.y << !is_tile_out_of_index(tile_index) << '\n';
                if(!is_tile_out_of_index(tile_index)){
                    place_an_ally(tile_index);
                    particles->emitParticle(get_tile_center_from_index(tile_index), 20);
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
    mapState = makeMapState(level);
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
                particles->emitParticle(vec2(xpos,ypos), 20);
            }
        }
    }
}

namespace {
    void imgui_help_menu(){
        if (ImGui::CollapsingHeader("Help"))
        {
            ImGui::Text("ABOUT THIS DEMO:");
            ImGui::BulletText("Click ally dropdown and select a unit. Click map to add a unit");
            ImGuiHelpImage("help/place_ally.png");
            ImGui::BulletText("Click sandbox and then ally dropdown and select a unit. Click map to add a unit");
            ImGuiHelpImage("help/place_enemy.png");
        }
    }
}

void Game::imgui_level_selection_menu(){
    if (ImGui::CollapsingHeader("Select a level"))
    {
        if (ImGui::Button("Sandbox")) {
            level = Level::sandbox;
            restart(level);
        }
        if (ImGui::Button("level1")) { level = Level::level1;  restart(level);}
        if (ImGui::Button("level2")) { level = Level::level2;  restart(level);}
        if (ImGui::Button("level3")) { level = Level::level3;  restart(level);}
        if (ImGui::Button("level4")) { level = Level::level4;  restart(level);}
        if (ImGui::Button("level5")) { level = Level::level5;  restart(level);}
        if (ImGui::Button("debug path finding")) { level = Level::path_finding_debug;  restart(level);}
        if (ImGui::Button("Start Screen")) { level = Level::start_screen; restart(level); }
    }
};

void Game::imgui_battle_control_menu(){
    if (ImGui::CollapsingHeader("Battle"))
    {
        if (ImGui::Button("Start battle")) has_battle_started = true;
        if (ImGui::Button("Restart level")) restart(level);
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
    save_json("sandbox.json", json);
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
    nlohmann::json json;
    load_json("sandbox.json", json);
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

