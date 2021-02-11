//
// Created by grumpy on 2021-02-10.
//

#ifndef SALMON_LEVEL_STATE_HPP
#define SALMON_LEVEL_STATE_HPP
// observer
class LevelState{
//    TODO: remove after observer pattern part is marked
public: LevelState(unsigned int gold, unsigned int health_total, unsigned int damage_received_total);
    void update_gold();
    void update_health();
    void update_received_damage();

    unsigned int getGold() const;

    unsigned int getHealthTotal() const;

    unsigned int getReceivedDamageTotal() const;

private:
    unsigned int gold;
    unsigned int health_total;
    unsigned int received_damage_total;
};
#endif //SALMON_LEVEL_STATE_HPP
