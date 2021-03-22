#ifndef CORE_UNITMAP_HPP
#define CORE_UNITMAP_HPP

#include <cstdint>
#include "utils/grid.hpp"
#include "constants.hpp"

enum UnitType {
    human_terminator,
    human_monitor,
    human_archer,
    human_healer,
    ai_terminator,
    ai_monitor,
    ai_archer,
    ai_healer,
//    TODO: remove empty
    empty,
};
static const std::string get_unit_mesh_key[] =
        {
                "human_terminator",
                "human_monitor",
                "human_archer",
                "human_healer",
                "ai_terminator",
                "ai_monitor",
                "ai_archer",
                "ai_healer",
        };

using UnitMapState = Grid<UnitType>;
UnitMapState makeUnitState();
UnitType char_to_unitType(const char c);
UnitType int_to_unitType(int i);
#endif