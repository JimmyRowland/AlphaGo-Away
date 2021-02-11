#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "level_state.h"

class UnitFactory {
public:
    UnitFactory(LevelState& level_state);
    ECS::Entity create_unit(vec2 position);


private:
    LevelState& level_state;

};
