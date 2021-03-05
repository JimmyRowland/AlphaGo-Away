#pragma once

#include "core/common.hpp"
#include "core/registry.hpp"
#include <entt/entt.hpp>
#include "components/motion.hpp"
// A simple physics system that moves rigid bodies and checks for collision

void physicsUpdate(float elapsed_ms, vec2 window_size_in_game_units);

