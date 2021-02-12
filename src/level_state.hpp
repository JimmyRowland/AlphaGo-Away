//
// Created by grumpy on 2021-02-10.
//

#ifndef SALMON_LEVEL_STATE_HPP
#define SALMON_LEVEL_STATE_HPP
// observer
#include <gl3w.h>
#include <GLFW/glfw3.h>
#include "common.hpp"


class LevelStateSystem{
//    TODO: remove after observer pattern part is marked
public: LevelStateSystem(unsigned int gold, unsigned int health_total, unsigned int damage_received_total);
    bool update_gold();
    void update_health();
    void update_received_damage();
    
    // observer function
    void on_collision(ECS::Entity entity_i, ECS::Entity entity_j);

    unsigned int getGold() const;

    unsigned int getHealthTotal() const;

    unsigned int getReceivedDamageTotal() const;

private:
    unsigned int gold;
    unsigned int health_total;
    unsigned int received_damage_total;
};
#endif //SALMON_LEVEL_STATE_HPP
