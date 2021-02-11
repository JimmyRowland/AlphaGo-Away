#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "level_state.hpp"

class UnitFactory {
public:
    UnitFactory(LevelStateSystem &level_state);
    ECS::Entity create_unit(vec2 position);


private:
    LevelStateSystem& level_state;

};

struct Unit
{
};
