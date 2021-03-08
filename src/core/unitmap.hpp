#ifndef CORE_UNITMAP_HPP
#define CORE_UNITMAP_HPP

#include <cstdint>
#include "utils/grid.hpp"
#include "constants.hpp"

enum class UnitType : std::uint8_t {
    human_terminator,
    human_monitor,
    human_archer,
    human_healer,
    ai_terminator,
    ai_monitor,
    ai_archer,
    ai_healer,
    empty,
};
using UnitMapState = Grid<UnitType>;
UnitMapState makeUnitState();
UnitType char_to_unitType(const char c);
#endif