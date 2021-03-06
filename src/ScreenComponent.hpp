#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

enum COMPONENT_TYPE {
    BASICBG, BUTTON
};

struct ScreenComponent
{
    // Creates all the associated render resources and default transform
    // Pass different path of the texture for different enemies. e.g., "ai_default.png" / "ai_default_2.png"
    static ECS::Entity createScreenComponent(vec2 position, std::string texture_path, vec2 size, float depth, COMPONENT_TYPE c_type);
};

struct ButtonComponent
{
    // to show the screen component is a button
};
