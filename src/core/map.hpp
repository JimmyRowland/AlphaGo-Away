#ifndef CORE_MAZE_HPP
#define CORE_MAZE_HPP

#include <cstdint>
#include "utils/grid.hpp"
#include "constants.hpp"
#include "factories.hpp"

//Copied from entt pacman

enum class TileType : std::uint8_t {
    basic, water, forest
};

using MapState = Grid<TileType>;
//using UnitMapState = Grid<UnitType>;
TileType char_to_tileType(const char c);
MapState makeMapState();
//UnitMapState makeUnitState();
char tileType_to_char(TileType tileType);
vec2 get_tile_center_from_position(vec2 position);
vec2 get_tile_center_from_index(ivec2 tile_index);
ivec2 get_tile_index(vec2 position);
bool is_tile_out_of_index(ivec2 index);
#endif
