
#include "unitmap.hpp"

namespace {
template <std::size_t Size>
void stateStr_to_unitType(UnitMapState &state, const char (&str)[Size]) {
    assert(state.area() + 1 == Size);
    for (std::size_t i = 0; i != Size - 1; ++i) {
        state[i] = char_to_unitType(str[i]);
    }
}
}


UnitType char_to_unitType(const char c) {
    switch (c) {
        case ' ': return UnitType::empty;
        case '1': return UnitType::human_terminator;
        case '2': return UnitType::human_monitor;
        case '3': return UnitType::human_archer;
        case '4': return UnitType::human_healer;
        case '5': return UnitType:: ai_terminator;
        case '6': return UnitType:: ai_monitor;
        case '7': return UnitType:: ai_archer;
        case '8': return UnitType:: ai_healer;
        default:
            assert(false);
            return {};
    }
}


UnitMapState makeUnitState() {
    UnitMapState state{tile_matrix_dimension};
    const char str[] = {
            "1              "
            "2              "
            "3              "
            "4              "
            "              5"
            "              6"
            "              7"
            "              8"
            "               "
            "               "
    };
    stateStr_to_unitType(state, str);
    return state;
}