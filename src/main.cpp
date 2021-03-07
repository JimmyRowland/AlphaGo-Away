// stlib
#include <chrono>
#include <iostream>

// internal
#include "core/common.hpp"
#include "core/game.hpp"
#include "system/render.hpp"
#include "system/physics.hpp"
#include "system/ai.hpp"
#include "logger/debug.hpp"
#include "core/constants.hpp"
#include "gui/imgui.h"
#include "gui/imgui_impl_glfw.h"
#include "gui/imgui_impl_opengl3.h"

using Clock = std::chrono::high_resolution_clock;
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
// Entry point
int main()
{
//    glfwSetErrorCallback(glfw_error_callback);
//    if (!glfwInit())
//        return 1;
//#ifdef __APPLE__
//    // GL 3.2 + GLSL 150
//    const char* glsl_version = "#version 150";
//#else
//    // GL 3.0 + GLSL 130
//    const char* glsl_version = "#version 130";
//
//#endif
//    GLFWwindow* window = glfwCreateWindow(1200, 800, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
//    if (window == NULL)
//        return 1;

    // Initialize the main systems
	Game game(window_size_in_px);
	RenderSystem renderer(*game.window);


	// Set all states to default
	auto t = Clock::now();

//    glfwMakeContextCurrent(window);
//	ImGui::CreateContext();
//    ImGuiIO& io = ImGui::GetIO(); (void)io;
//    ImGui::StyleColorsDark();
//    ImGui_ImplGlfw_InitForOpenGL(window, true);
//    ImGui_ImplOpenGL3_Init(glsl_version);
//    bool show_demo_window = true;
//    bool show_another_window = false;
//    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
//

    game.restart();
	// Variable timestep loop
	while (!game.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
//		glfwPollEvents();
//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui_ImplGlfw_NewFrame();
//        ImGui::NewFrame();
//        if (show_demo_window)
//            ImGui::ShowDemoWindow(&show_demo_window);
//
//        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
//        {
//            static float f = 0.0f;
//            static int counter = 0;
//
//            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
//
//            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
//            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
//            ImGui::Checkbox("Another Window", &show_another_window);
//
//            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
//            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
//
//            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
//                counter++;
//            ImGui::SameLine();
//            ImGui::Text("counter = %d", counter);
//
//            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//            ImGui::End();
//        }
//
//        // 3. Show another simple window.
//        if (show_another_window)
//        {
//            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
//            ImGui::Text("Hello from another window!");
//            if (ImGui::Button("Close Me"))
//                show_another_window = false;
//            ImGui::End();
//        }
		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms = static_cast<float>((std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count()) / 1000.f;
		t = now;
        DebugSystem::clearDebugComponents();
        aiUpdate(elapsed_ms, window_size_in_game_units);
        game.update(elapsed_ms, window_size_in_game_units);
        physicsUpdate(elapsed_ms, window_size_in_game_units);
        renderer.draw(window_size_in_game_units);

//        ImGui::Render();
//        int display_w, display_h;
//        glfwGetFramebufferSize(window, &display_w, &display_h);
//        glViewport(0, 0, display_w, display_h);
//        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
//        glClear(GL_COLOR_BUFFER_BIT);
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//        glfwSwapBuffers(window);
	}
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
	m_registry.clear<>();



    return EXIT_SUCCESS;
}
