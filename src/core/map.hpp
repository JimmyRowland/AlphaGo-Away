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

MapState makeMapState();


ivec2 get_tile_index(vec2 position);
#endif
