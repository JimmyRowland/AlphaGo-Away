//
// Created by grumpy on 2021-03-06.
//
#include "gui.hpp"
void level_selection_menu(const std::function<void()>& on_select_sandbox){
    {
        ImGui::Begin("Select a level");
        if (ImGui::Button("Sandbox")) on_select_sandbox();
        ImGui::End();
    }
};