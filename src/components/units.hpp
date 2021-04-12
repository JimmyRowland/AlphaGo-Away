//
// Created by grumpy on 2021-03-05.
//

#ifndef ALPHAGO_AWAY_UNITS_HPP
#define ALPHAGO_AWAY_UNITS_HPP
#include "core/unitmap.hpp"
struct Terminator{
};
struct Archer{
};
struct Monitor{
};
struct Healer{
};
struct Attack{};
struct Walk{};
struct Stand{};
struct Die{};
struct Explosion{};

struct Enemy{

};
struct Ally{

};
struct EnemyProjectile{

};
struct AllyProjectile{

};
struct UnitProperty{
    int hp = 100;
    int maxhp = 100;
    int attackRange = 5;
    int moveRange = 5;
    int damage = 5;
    UnitType unit_type = UnitType::human_terminator;
//    entt::entity desiredTarget;
    entt::entity actualTarget = entt::null;
    std::vector<std::pair<int, int>> path = {};
    float close_combat_damage_modifier = 1.f;
};

struct ProjectileProperty{

    UnitType unit_type = UnitType::human_terminator;
//    entt::entity desiredTarget;
    entt::entity actualTarget = entt::null;
    float damage;
};

struct ProjectileTimer{

    float timer = 30;
}; 

struct CameraShake 
{
	float time = 0;
};
#endif //ALPHAGO_AWAY_UNITS_HPP
