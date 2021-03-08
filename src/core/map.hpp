#ifndef CORE_MAZE_HPP
#define CORE_MAZE_HPP

#include <cstdint>
#include "utils/grid.hpp"
#include "constants.hpp"

//Copied from entt pacman

enum class TileType : std::uint8_t {
    basic, water, forest
};

using MapState = Grid<TileType>;
TileType char_to_tileType(const char c);
MapState makeMapState();
char tileType_to_char(TileType tileType);

ivec2 get_tile_index(vec2 position);
#endif
