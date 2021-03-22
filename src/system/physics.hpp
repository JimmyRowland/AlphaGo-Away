#ifndef ALPHAGO_AWAY_PHYSICS_HPP
#define ALPHAGO_AWAY_PHYSICS_HPP
#include "core/common.hpp"
#include "core/registry.hpp"
#include "components/units.hpp"
#include <entt/entt.hpp>
#include "components/motion.hpp"
#include "system/utils/kd-tree.hpp"
#include "entities/unit.hpp"
// A simple physics system that moves rigid bodies and checks for collision

void physics_update(float elapsed_ms);
bool is_out_of_boundary(entt::entity entity);
#endif //ALPHAGO_AWAY_PHYSICS_HPP