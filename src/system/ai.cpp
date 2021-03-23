// internal
#include "ai.hpp"

namespace {
    float cooldown = 0.f;

    float get_entity_distance(entt::entity entity, entt::entity target_entity) {
        auto &position_i = m_registry.get<Position>(entity);
        auto &position_j = m_registry.get<Position>(target_entity);
        return glm::distance(position_j.position, position_i.position);
    }

    bool within_attack_range(entt::entity entity, entt::entity target_entity) {
        if (m_registry.valid(entity) && m_registry.valid(target_entity)) {
            auto &property = m_registry.get<UnitProperty>(entity);
            if (get_entity_distance(entity, target_entity) < tile_size.x * 1.5*property.attackRange) {
                return true;
            }
        }
        return false;
    }

    bool is_terminator(entt::entity entity, entt::entity target_entity){
        return m_registry.has<Terminator>(entity);
    }

    void resolve_damage(entt::entity entity, entt::entity target_entity) {
        if(is_terminator(entity, target_entity)){
            auto &&[property, position] = m_registry.get<UnitProperty, Position>(entity);
            auto &&[target_property, target_position] = m_registry.get<UnitProperty, Position>(target_entity);
            target_property.hp -= property.damage * property.close_combat_damage_modifier;
            property.hp -= target_property.damage * target_property.close_combat_damage_modifier;
            //Once the enemy be eliminated, the projectiles also gone

            if (target_property.hp <= 0) {
                explosion_factory(target_position.position);
                m_registry.destroy(target_entity);

            }
            if (property.hp <= 0) {
                explosion_factory(position.position);
                m_registry.destroy(entity);
            }
        }
    }

    void set_targets() {
        for (auto &&[entity, property, position]: m_registry.view<UnitProperty, Position, Enemy>().each()) {
            float min_dist = 99999999.f;
            for (auto &&[target_entity, target_property, target_position]: m_registry.view<UnitProperty, Position, Ally>().each()) {
                auto dist = glm::distance(position.position, target_position.position);
                if (dist < min_dist) {
                    min_dist = dist;
                    property.actualTarget = target_entity;
                }
            }
        }

        for (auto &&[entity, property, position]: m_registry.view<UnitProperty, Position, Ally>().each()) {
            float min_dist = 99999999.f;
            for (auto &&[target_entity, target_property, target_position]: m_registry.view<UnitProperty, Position, Enemy>().each()) {
                auto dist = glm::distance(position.position, target_position.position);
                if (dist < min_dist) {
                    min_dist = dist;
                    property.actualTarget = target_entity;
                }
            }
        }
    }

    void set_path() {
        A_Star a = A_Star(std::make_pair(tile_matrix_dimension.x, tile_matrix_dimension.y));
        for (auto &&[entity, property, position]: m_registry.view<UnitProperty, Position>().each()) {
            if (!m_registry.valid(property.actualTarget) || property.actualTarget == entity || within_attack_range(entity,  property.actualTarget)) {
                property.path = {};
            } else {
                if (m_registry.valid(property.actualTarget)) {
                    auto&&[target_position, target_property] = m_registry.get<Position, UnitProperty>(
                            property.actualTarget);
                    auto target_tile_index = target_property.path.empty() ? get_tile_index(target_position.position): rand() % 10 > 5 ? ivec2(target_property.path[0].first, target_property.path[0].second): get_tile_index(target_position.position);                    auto entity_tile_index = get_tile_index(position.position);
                    property.path = a.getPath(std::make_pair(entity_tile_index.x, entity_tile_index.y),
                                              std::make_pair(target_tile_index.x, target_tile_index.y));


                }
            }
        }
    }

    void set_projectile_targets() {
        for (auto &&[entity, property, position]: m_registry.view<ProjectileProperty, Position, EnemyProjectile>().each()) {
            float min_dist = 99999999.f;
            for (auto &&[target_entity, target_property, target_position]: m_registry.view<UnitProperty, Position, Ally>().each()) {
                auto dist = glm::distance(position.position, target_position.position);
                if (dist < min_dist) {
                    min_dist = dist;
                    property.actualTarget = target_entity;
                }
            }
        }

        for (auto &&[entity, property, position]: m_registry.view<ProjectileProperty, Position, AllyProjectile>().each()) {
            float min_dist = 99999999.f;
            for (auto &&[target_entity, target_property, target_position]: m_registry.view<UnitProperty, Position, Enemy>().each()) {
                auto dist = glm::distance(position.position, target_position.position);
                if (dist < min_dist) {
                    min_dist = dist;
                    property.actualTarget = target_entity;
                }
            }
        }
    }

    void update_state() {
        for (auto &&[entity, property]: m_registry.view<UnitProperty>().each()) {
            if (m_registry.valid(entity)) {
                if (within_attack_range(entity, property.actualTarget)) {
                    //stop at the proper attack range and then attack
                    property.path={};
                    // eject projectiles
                    projectile_factory(entity, property.unit_type, property.actualTarget);
                    unit_attack(entity, property.unit_type);
                    resolve_damage(entity, property.actualTarget);
                } else if (!property.path.empty()) {
                    unit_walk(entity, property.unit_type);
                } else {
                    unit_stand(entity, property.unit_type);
                }
            }
        }
    }



    void clear_explosions() {

        for (auto &entity: m_registry.view<Explosion>()) {
            m_registry.destroy(entity);
        }

    }
}

void ai_update(float elapsed_ms) {
    cooldown -= elapsed_ms;
    if (cooldown <= 0.f) {
        cooldown += cool_down_unit;
        clear_explosions();
        set_targets();
        set_path();
        set_projectile_targets();
       // set_projectile_path();
        update_state();
//        clear_projectile(elapsed_ms);

    }

}