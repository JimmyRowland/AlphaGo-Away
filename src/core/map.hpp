#ifndef CORE_MAZE_HPP
#define CORE_MAZE_HPP

#include <cstdint>
#include "utils/grid.hpp"

//Copied from entt pacman

enum class TileType : std::uint8_t {
    basic, water, forest
};

using MapState = Grid<TileType>;

MapState makeMapState();

#endif
