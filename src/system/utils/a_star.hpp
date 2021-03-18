#include "core/common.hpp"
#include "core/registry.hpp"
#include "components/motion.hpp"
#include <iostream>
#include <utility>
#include <vector>
#include <iterator>
#include <queue>
#include <map>

#ifndef A_STAR_H
#define A_STAR_H

class A_Star {
public:
    std::vector<std::vector<int >> grid = {};
    // update the map as needed this is just filler
    // TODO: update this to use a centralized enum along with battle_screen_world.cpp
    std::map<TileType, int> costs = {
            {TileType::basic,  1},
            {TileType::forest, 999},
            {TileType::water,  999}
    };

    // comparator class for priority queue
    class compare {
    public:
        int operator()(const std::pair<int, std::pair<int, int>> &p1, const std::pair<int, std::pair<int, int>> &p2) {
            return p1.first > p2.first;
        }
    };

    // manhattan distance, we can use a different heuristic if there is a preference
    float heuristic(std::pair<int, int> a, std::pair<int, int> b) {
        return abs(a.first - b.first) + abs(a.second - b.second);
    }

    // initialize a 2D vector to represent the grid, where grid[i][j] is the cost of moving to grid[i][j] from an adjacent square
    A_Star(std::pair<int, int> grid_size) {
        for (auto tile : m_registry.view<Tile>()) {
            this->grid.resize(grid_size.first);
            for (unsigned int i = 0; i < grid.size(); i++) {
                this->grid[i].resize(grid_size.second);
            }

        }
        for (auto&&[entity, tile_comp, tile_pos] : m_registry.view<Tile, Position>().each()) {
            ivec2 tile_index = get_tile_index(tile_pos.position);
            this->grid[tile_index.x][tile_index.y] = costs[tile_comp.type];
        }

    }

    // a* search algorithm, start is a the starting grid position, end is the target grid position
    std::vector<std::pair<int, int>> getPath(std::pair<int, int> start, std::pair<int, int> end) {
        std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, compare> fronteir = {};
        std::map<std::pair<int, int>, int> costSoFar = {};
        std::map<std::pair<int, int>, std::pair<int, int>> cameFrom = {};
        std::vector<std::pair<int, int>> res = {};

        fronteir.push(std::make_pair(0, start));
        costSoFar[start] = 0;
        while (!fronteir.empty()) {
            std::vector<std::pair<int, int>> neighbours;
            std::pair<int, int> current = fronteir.top().second;
            fronteir.pop();
            if (current == end) {
                break;
            }
            // check neighbours, can be modified to add diagonal neighbours in the future
            neighbours.push_back(std::make_pair(current.first - 1, current.second));
            neighbours.push_back(std::make_pair(current.first + 1, current.second));
            neighbours.push_back(std::make_pair(current.first, current.second - 1));
            neighbours.push_back(std::make_pair(current.first, current.second + 1));

            for (auto &neighbour : neighbours) {
                if (neighbour.first >= 0 && neighbour.first < grid.size() && neighbour.second >= 0 &&
                    neighbour.second < grid[0].size()) {
                    int updatedCost = costSoFar[current] + this->grid[neighbour.first][neighbour.second];
                    if (!costSoFar.count(neighbour) || updatedCost < costSoFar[neighbour]) {
                        costSoFar[neighbour] = updatedCost;
                        int prioirity = updatedCost + heuristic(neighbour, end);
                        fronteir.push(std::make_pair(prioirity, neighbour));
                        cameFrom[neighbour] = current;
                    }
                }

            }
        }

        std::pair<int, int> current = end;
        while (current != start) {
            res.insert(res.begin(), current);
            current = cameFrom[current];
        }

        return res;

    }


};

#endif