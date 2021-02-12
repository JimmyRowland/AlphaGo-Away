#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "level_state.hpp"
#include "keyboard_observer.hpp"
const unsigned int SHOULD_PLACE_ENEMY = 1;
const unsigned int SHOULD_PLACE_HUMAN = 0;
class UnitFactory: public KeyboardObserver {
public:
    UnitFactory(LevelStateSystem &level_state);
    ECS::Entity create_unit(vec2 position);
    void on_key_click(int key, int action);

private:
    LevelStateSystem& level_state;
    unsigned int should_place_enemy = SHOULD_PLACE_HUMAN;
};


struct Unit
{
};
