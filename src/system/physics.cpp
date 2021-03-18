// internal
#include "physics.hpp"

#include "logger/debug.hpp"

namespace {
    vec2 get_bounding_box(const Position &position) {
        return {abs(position.scale.x), abs(position.scale.y)};
    }

    bool satHelper(const entt::entity e1, const entt::entity e2) {
//        if(!(m_registry.has<BoundingBox>(e1) && m_registry.has<BoundingBox>(e2))) return false;
        BoundingBox bb1 = m_registry.get<BoundingBox>(e1); // bounding box for first entity
        BoundingBox bb2 = m_registry.get<BoundingBox>(e2); // bounding box for second entity
        if (bb1.vertices.size() <= 0 || bb2.vertices.size() <= 0) { // all unit entities should have a bounding box
            return false;
        }
        for (int i = 0; i < bb1.transformed_vertices.size(); i++) {
            auto current = bb1.transformed_vertices[i];
            auto next = bb1.transformed_vertices[(i + 1) % bb1.transformed_vertices.size()];
            auto edge = next - current;
            vec2 axis = vec2(-edge.y, edge.x);

            auto e1MaxProj = NULL;
            auto e2MaxProj = NULL;
            auto e1MinProj = NULL;
            auto e2MinProj = NULL;
            for (vec2 v : bb1.transformed_vertices) { // project entity 1's bb vertices onto the axis
                auto projection = dot(axis, v);
                if (e1MaxProj == NULL || projection > e1MaxProj) {
                    e1MaxProj = projection;
                }
                if (e1MinProj == NULL || projection < e1MinProj) {
                    e1MinProj = projection;
                }
            }
            for (vec2 v : bb2.transformed_vertices) { // project entity 2's bb vertices onto the axis
                auto projection = dot(axis, v);
                if (e2MaxProj == NULL || projection > e2MaxProj) {
                    e2MaxProj = projection;
                }
                if (e2MinProj == NULL || projection < e2MinProj) {
                    e2MinProj = projection;
                }
            }
            if (e1MaxProj < e2MinProj || e1MinProj > e2MaxProj) {
                return false;
            }
        }
        return true;
    }

    bool collides(const entt::entity e1, const entt::entity e2) {
        bool e1Check = satHelper(e1, e2);
        bool e2Check = satHelper(e2, e1);
        return (e1Check && e2Check);
    }

    void update_velocity_and_facing_dir(entt::entity entity_i, entt::entity entity_j) {
        auto &&[position_i, motion_i, property_i] = m_registry.get<Position, Motion, UnitProperty>(entity_i);
        auto &&[position_j, motion_j] = m_registry.get<Position, Motion>(entity_j);
        if (!property_i.path.empty()) {
            std::pair<int, int> nextStep = property_i.path[0];
            auto tile_index = get_tile_index(position_i.position);
            if (tile_index.x == nextStep.first && tile_index.y == nextStep.second) {
                property_i.path.erase(property_i.path.begin());
            }
            vec2 next_position = get_tile_center_from_index(ivec2(nextStep.first, nextStep.second));
            motion_i.velocity = glm::normalize(next_position - position_i.position) * property_i.max_velocity;
        }

        if (motion_i.velocity.x != 0) {
            position_i.scale.x = -1 * sign(motion_i.velocity.x) * abs(position_i.scale.x);
        } else if ((position_j.position - position_i.position).x > 0) {
            position_i.scale.x = -abs(position_i.scale.x);
        } else {
            position_i.scale.x = abs(position_i.scale.x);
        }
    }

    void set_transformed_bounding_box(entt::entity entity) {
        if (m_registry.has<BoundingBox, Position>(entity)) {
            auto&&[bb, position] = m_registry.get<BoundingBox, Position>(entity);
            Transform transform;
            transform.translate(position.position);
            transform.rotate(position.angle);
            transform.scale(position.scale);
            std::vector<vec2> temp_vector;
            for (int i = 0; i < bb.vertices.size(); i++) {
                auto transformed = transform.mat * vec3(bb.vertices[i].x, bb.vertices[i].y, 1.f);
                temp_vector.emplace_back(transformed.x, transformed.y);
                bb.transformed_vertices = temp_vector;
            }
        }
    }

    void on_collision_resolve_damage(entt::entity entity_i, entt::entity entity_j) {
        UnitProperty &property_i = m_registry.get<UnitProperty>(entity_i);
        UnitProperty &property_j = m_registry.get<UnitProperty>(entity_j);
        bool is_entity_i_enemy = m_registry.has<Enemy>(entity_i);
        bool is_entity_j_enemy = m_registry.has<Enemy>(entity_j);
        if (is_entity_j_enemy != is_entity_i_enemy) {
            unit_attack(entity_i, UnitType::ai_terminator);
            unit_attack(entity_j, UnitType::ai_terminator);
            property_i.hp -= property_j.damage;
            property_j.hp -= property_i.damage;
            if (property_i.hp <= 0) {
                m_registry.destroy(entity_i);
//                unit_stand(entity_j, UnitType::ai_terminator);
            }
            if (property_j.hp <= 0) {
                m_registry.destroy(entity_j);
//                unit_stand(entity_i, UnitType::ai_terminator);
            }
        }
    }

    void boundry_checking(entt::entity entity) {
        auto &&[position, motion] = m_registry.get<Position, Motion>(entity);
        if (position.position.y < map_y_min) {
            position.position.y = map_y_min;
            if (motion.velocity.y < 0) {
                motion.velocity.y = 0;
            }
        } else if (position.position.y > map_y_max) {
            position.position.y = map_y_max;
            if (motion.velocity.y > 0) {
                motion.velocity.y = 0;
            }
        }
        if (position.position.x < map_x_min) {
            position.position.x = map_x_min;
            if (motion.velocity.x < 0) {
                motion.velocity.x = 0;
            }
        } else if (position.position.x > map_x_max) {
            position.position.x = map_x_max;
            if (motion.velocity.x > 0) {
                motion.velocity.x = 0;
            }
        }
    }


}


void physics_update(float elapsed_ms) {

    if (DebugSystem::in_debug_mode) {
        for (auto&&[entity, boundingBox] :m_registry.view<BoundingBox>().each()) {
            if (m_registry.valid(entity)) {
                for (auto &vertices : boundingBox.transformed_vertices) {
                    auto dotSize = vec2(5.f, 5.f);
                    DebugSystem::createLine(vertices, dotSize);
                }
            }
        }
    }
    float step_seconds = 1.0f * (elapsed_ms / 1000.f);
    for (auto&&[entity, motion, position, unit_property, bounding_box]: m_registry.view<Motion, Position, UnitProperty, BoundingBox>().each()) {
        if (m_registry.valid(unit_property.actualTarget) &&
            m_registry.has<UnitProperty, Motion, Position, BoundingBox>(unit_property.actualTarget)) {
            update_velocity_and_facing_dir(entity, unit_property.actualTarget);
            position.position += motion.velocity * step_seconds;
        }
        set_transformed_bounding_box(entity);
        boundry_checking(entity);
    }


//        TODO move to ai
    auto entities = m_registry.view<UnitProperty>();
    for (int i = 0; i < entities.size(); i++) {
        auto entity_i = entities[i];
        if (m_registry.valid(entity_i)) {
            if (m_registry.has<UnitProperty, Motion, Position, BoundingBox>(entity_i)) {
                auto &&[motion_i, position_i, unit_property_i] = m_registry.get<Motion, Position, UnitProperty>(
                        entity_i);
                bool isEnemy = m_registry.has<Enemy>(entity_i);
                KD_Tree kdtree = KD_Tree(!isEnemy);
                kdtree.nearest(kdtree.root, entity_i, unit_property_i.actualTarget);
                if (m_registry.valid(unit_property_i.actualTarget)) {
//                    If target is not in attack range
                    auto targetProperty = m_registry.get<UnitProperty>(unit_property_i.actualTarget);
                    if (targetProperty.hp == targetProperty.maxhp) {
                        unit_walk(entity_i, UnitType::ai_terminator);
                    }
                    for (unsigned int j = i + 1; j < entities.size(); j++) {
                        auto entity_j = entities[j];
                        if (m_registry.valid(entity_j) && m_registry.valid(entity_i)) {
                            if (m_registry.has<UnitProperty, Motion, Position, BoundingBox>(entity_j)) {
                                auto &&[motion_j, position_j, unit_property_j] = m_registry.get<Motion, Position, UnitProperty>(
                                        entity_j);
//                   TODO iterate over m_register.view<Enemy/Ally> instead
                                if (collides(entity_i, entity_j)) {
//                                    vec2 direction = position_j.position - position_i.position;
//                                    if (direction.x == 0 && direction.y == 0) {
//                                        direction.x = 20;
//                                    }
//                                    motion_i.velocity = direction * step_seconds * -10.f;
//                                    motion_j.velocity = direction * step_seconds * 10.f;
                                    on_collision_resolve_damage(entity_i, entity_j);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
