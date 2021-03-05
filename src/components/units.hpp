//
// Created by grumpy on 2021-03-05.
//

#ifndef ALPHAGO_AWAY_UNITS_HPP
#define ALPHAGO_AWAY_UNITS_HPP
struct GroundUnit{

};
struct RangedUnit{

};
struct Projectile{

};
struct Enemy{

};
struct Ally{

};
struct UnitProperty{
    int hp = 100;
    int maxhp = 100;
    int attackRange = 5;
    int moveRange = 5;
    int damage = 5;
    entt::entity desiredTarget;
    entt::entity actualTarget;
};
#endif //ALPHAGO_AWAY_UNITS_HPP
