#ifndef ALPHAGO_AWAY_PHYSICS_HPP
#define ALPHAGO_AWAY_PHYSICS_HPP
#include "core/common.hpp"
#include "core/registry.hpp"
#include "components/units.hpp"
#include <entt/entt.hpp>
#include "components/motion.hpp"
#include "core/kd-tree.hpp"
#include "entities/unit.hpp"
// A simple physics system that moves rigid bodies and checks for collision

void physicsUpdate(float elapsed_ms, vec2 window_size_in_game_units);

#endif //ALPHAGO_AWAY_PHYSICS_HPP