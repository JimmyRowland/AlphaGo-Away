//
//  maze.cpp
//  EnTT Pacman
//
//  Created by Indiana Kernick on 22/9/18.
//  Copyright © 2018 Indiana Kernick. All rights reserved.
//

#include "map.hpp"

#include "constants.hpp"
#include "common.hpp"
namespace {
template <std::size_t Size>
void stateStr(MapState &state, const char (&str)[Size]) {
  assert(state.area() + 1 == Size);
  for (std::size_t i = 0; i != Size - 1; ++i) {
    state[i] = char_to_tileType(str[i]);
  }
}

}
TileType char_to_tileType(const char c) {
    switch (c) {
        case ' ': return TileType::basic;
        case '#': return TileType::forest;
        case '-': return TileType::water;
        default:
            assert(false);
            return {};
    }
}
MapState makeMapState() {
  MapState state{tile_matrix_dimension};
//  const char str[] = {
//    "###############"
//    "#        #     "
//    "# ## ### # ### "
//    "# ## ### # ### "
//    "#              "
//    "# ## # ##### # "
//    "#    #   #   # "
//    "#### ### # ### "
//    "   # #       # "
//    "#### # ----- # "
//  };

    const char str[] = {
           "###############"
           "###############"
           "###############"
           "###############"
           "###############"
           "###############"
           "###############"
           "###############"
           "###############"
           "###############"
    };
  stateStr(state, str);
  return state;
}

ivec2 get_tile_index(vec2 position){
    return ivec2(floor(position.x / tile_size.x), floor(position.y / tile_size.y));
}

char tileType_to_char(TileType tileType){
    switch (tileType) {
        case TileType::basic: return ' ';
        case TileType::forest: return '#';
        case TileType::water: return '-';
        default:
            assert(false);
            return {};
    }
}