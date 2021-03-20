//copied from entt pacman
#include "map.hpp"

namespace {
template <std::size_t Size>
void stateStr_to_tileType(MapState &state, const char (&str)[Size]) {
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

MapState makeMapStatePathFinding() {
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
            " #             "
            "               "
            " ##########    "
            " #          ###"
            " # ##########  "
            "##          #  "
            "########### #  "
            "            #  "
            " ############  "
            "               "
    };
    stateStr_to_tileType(state, str);
    return state;
}

MapState makeMapState(Level level) {
    if(level == Level::path_finding_debug) return makeMapStatePathFinding();
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
           "               "
           "               "
           "               "
           "               "
           "               "
           "               "
           "               "
           "               "
           "               "
           "               "
    };
    stateStr_to_tileType(state, str);
  return state;
}


ivec2 get_tile_index(vec2 position){
    return ivec2(floor(position.x / tile_size.x), floor(position.y / tile_size.y));
}

vec2 get_tile_center_from_position(vec2 position){
    vec2 index = get_tile_index(position);
    return index*tile_size+(tile_size/2.f);
};
vec2 get_tile_center_from_index(ivec2 tile_index){
    vec2 index_in_float = tile_index;
    return index_in_float * tile_size + (tile_size / 2.f);
};

bool is_tile_out_of_index(ivec2 tile_index){
    return !(tile_index.x > -1 && tile_index.x < tile_matrix_dimension.x && tile_index.y > -1 && tile_index.y < tile_matrix_dimension.y);
};

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