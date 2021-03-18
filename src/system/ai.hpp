#ifndef ALPHAGO_AWAY_AI_HPP
#define ALPHAGO_AWAY_AI_HPP
#include <vector>
#include "system/utils/a_star.hpp"
#include "core/common.hpp"
#include <entt/entt.hpp>
#include "core/registry.hpp"
#include "components/units.hpp"
#include "components/tile.hpp"
#include "components/motion.hpp"
void ai_update(float elapsed_ms);
#endif //ALPHAGO_AWAY_AI_HPP