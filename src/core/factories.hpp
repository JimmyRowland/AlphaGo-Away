#ifndef ALPHAGO_AWAY_FACTORIES_HPP
#define ALPHAGO_AWAY_FACTORIES_HPP
#include "core/common.hpp"
#include <system/render.hpp>
#include "registry.hpp"
#include "components/units.hpp"
#include "core/map.hpp"
#include "components/tile.hpp"
#include "constants.hpp"
enum class UnitType : std::uint8_t {
    human_terminator,
    human_monitor,
    human_archer,
    human_healer,
    ai_terminator,
    ai_monitor,
    ai_archer,
    ai_healer,
    empty,
};

entt::entity unit_factory(vec2 pos, UnitType unitType = UnitType::human_terminator);
entt::entity tile_factory(vec2 pos, TileType tileType);
void swap_tile_texture(entt::entity tile, TileType tileType);
GLuint get_tile_texture_id(TileType tileType);

#endif //ALPHAGO_AWAY_FACTORIES_HPP
