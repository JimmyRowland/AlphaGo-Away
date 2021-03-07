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

TileType stateChar(const char c) {
  switch (c) {
    case ' ': return TileType::basic;
    case '#': return TileType::forest;
    case '-': return TileType::water;
    default:
      assert(false);
      return {};
  }
}

template <std::size_t Size>
void stateStr(MapState &state, const char (&str)[Size]) {
  assert(state.area() + 1 == Size);
  for (std::size_t i = 0; i != Size - 1; ++i) {
    state[i] = stateChar(str[i]);
  }
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