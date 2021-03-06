// Header
#include "debug.hpp"
#include "render_components.hpp"
#include "start_screen_world.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>

// Game configuration
// TODO: Add any needed contant here

// Create the start screen world
// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
StartWorldSystem::StartWorldSystem(ivec2 window_size_px) :
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
    auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((StartWorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
    auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((StartWorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
    glfwSetKeyCallback(window, key_redirect);
    glfwSetCursorPosCallback(window, cursor_pos_redirect);

    // Playing background music indefinitely
    init_audio();
    Mix_PlayMusic(background_music, -1);
    std::cout << "Loaded music\n";
}

StartWorldSystem::~StartWorldSystem(){
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

void StartWorldSystem::init_audio()
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


// Should the game be over ?
bool StartWorldSystem::is_over() const
{
    return glfwWindowShouldClose(window)>0;
}

// On key callback
// TODO A1: check out https://www.glfw.org/docs/3.3/input_guide.html
void StartWorldSystem::on_key(int key, int, int action, int mod)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Add keyboard contorl here if any.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    

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


void StartWorldSystem::on_mouse_move(vec2 mouse_pos)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Add mouse contorl here.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
        (void)mouse_pos;
}
