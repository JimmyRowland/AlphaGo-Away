//
// Created by grumpy on 2021-02-10.
//
#include "common.hpp"
#include "tiny_ecs.hpp"
#ifndef SALMON_COLLISION_OBSERVER_HPP
#define SALMON_COLLISION_OBSERVER_HPP
class CollisionObserver{
public:
    virtual void on_collision(ECS::Entity entity_i, ECS::Entity entity_j) = 0;
};
#endif //SALMON_COLLISION_OBSERVER_HPP
