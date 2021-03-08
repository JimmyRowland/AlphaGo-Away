//
// Created by grumpy on 2021-03-06.
//
#define STB_IMAGE_IMPLEMENTATION
#include "gui.hpp"
void ImGuiImage(GLuint texture_id){
    ImGui::Image((void*)(intptr_t)texture_id, ImVec2(tile_size.x, tile_size.y));
}

