//
// Created by grumpy on 2021-02-10.
//

#include "level_state.hpp"

LevelStateSystem::LevelStateSystem(unsigned int gold, unsigned int health_total, unsigned int damage_received_total) {
    this->gold = gold;
    this->health_total = health_total;
    this->received_damage_total = damage_received_total;
}

bool LevelStateSystem::update_gold() {
    if (gold >= 100) {
        gold -= 100;
        return true;
    }
    else {
        gold += 50;
        return false;
    }
}

void LevelStateSystem::update_health() { if (health_total >= 10) health_total -= 10; }

void LevelStateSystem::update_received_damage() { if (received_damage_total >= 100) received_damage_total -= 100; }

unsigned int LevelStateSystem::getGold() const {
    return gold;
}

unsigned int LevelStateSystem::getHealthTotal() const {
    return health_total;
}

unsigned int LevelStateSystem::getReceivedDamageTotal() const {
    return received_damage_total;
}

void LevelStateSystem::on_collision(ECS::Entity entity_i, ECS::Entity entity_j) {
    int x = 0;
}

