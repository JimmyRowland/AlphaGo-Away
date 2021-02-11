//
// Created by grumpy on 2021-02-10.
//

#include "level_state.hpp"

LevelState::LevelState(unsigned int gold, unsigned int health_total, unsigned int damage_received_total) {
}
void LevelState::update_gold() {if(gold>=100) gold-=100;}

void LevelState::update_health() {if(health_total>=10) health_total-=10;}
void LevelState::update_received_damage() {if(received_damage_total>=100) received_damage_total-=100;}

unsigned int LevelState::getGold() const {
    return gold;
}

unsigned int LevelState::getHealthTotal() const {
    return health_total;
}

unsigned int LevelState::getReceivedDamageTotal() const {
    return received_damage_total;
}
