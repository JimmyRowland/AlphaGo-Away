#ifndef ALPHAGO_AWAY_FACTORIES_HPP
#define ALPHAGO_AWAY_FACTORIES_HPP
#include "core/common.hpp"
#include <system/render.hpp>
#include "factories.hpp"
#include "registry.hpp"
#include "components/units.hpp"
#include "map.hpp"
#include "components/tile.hpp"
#include "constants.hpp"
entt::entity ground_unit_factory(vec2 pos, bool should_place_enemy = false);
entt::entity tile_factory(vec2 pos, TileType tileType);
#endif //ALPHAGO_AWAY_FACTORIES_HPP
