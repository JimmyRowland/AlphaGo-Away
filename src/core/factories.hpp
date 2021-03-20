#ifndef ALPHAGO_AWAY_FACTORIES_HPP
#define ALPHAGO_AWAY_FACTORIES_HPP
#include "core/common.hpp"
#include <system/render.hpp>
#include "registry.hpp"
#include "components/units.hpp"
#include "components/tile.hpp"
#include "constants.hpp"
#include "unitmap.hpp"
#include "components/ui.hpp"
#include "entities/unit.hpp"


void init_factories();
entt::entity unit_factory(vec2 pos, UnitType unitType = UnitType::human_terminator);
entt::entity tile_factory(vec2 pos, TileType tileType);
void swap_tile_texture(entt::entity tile, TileType tileType);
GLuint get_tile_texture_id(TileType tileType);
void loading_screen_factory();
void background_factory();
#endif //ALPHAGO_AWAY_FACTORIES_HPP
