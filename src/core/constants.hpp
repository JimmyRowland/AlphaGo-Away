//
// Created by grumpy on 2021-03-04.
//

#ifndef ALPHAGO_AWAY_CONSTANTS_HPP
#define ALPHAGO_AWAY_CONSTANTS_HPP

constexpr ivec2 window_size_in_px = {1200, 800};
constexpr vec2 window_size_in_game_units = { 1200, 800 };
constexpr ivec2 tile_matrix_dimension = {15 , 10};
constexpr vec2 tile_size = {80, 80};
constexpr float map_x_min = 0;
constexpr float map_x_max = tile_matrix_dimension.x*tile_size.x;
constexpr float map_y_min = 0;
constexpr float map_y_max = tile_matrix_dimension.y*tile_size.y;
#endif //ALPHAGO_AWAY_CONSTANTS_HPP
