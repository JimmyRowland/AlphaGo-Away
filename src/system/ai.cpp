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
            if (get_entity_distance(entity, target_entity) < tile_size.x * 1.5*property.attackRange/2) {
                return true;
            }
        }
        return false;
    }

    void resolve_damage(entt::entity entity, entt::entity target_entity) {
        auto &&[property, position] = m_registry.get<UnitProperty, Position>(entity);
        auto &&[target_property, target_position] = m_registry.get<UnitProperty, Position>(target_entity);
        target_property.hp -= property.damage;
        property.hp -= target_property.damage;
        //Once the enemy be eliminated, the projectiles also gone
        auto &projectile = m_registry.get<Projectiles>(entity);
        if (target_property.hp <= 0) {
            explosion_factory(target_position.position);
            m_registry.destroy(target_entity);
            if(projectile.pro != entt::null && m_registry.valid(projectile.pro)){
            m_registry.destroy(projectile.pro);}
        }
        if (property.hp <= 0) {
            explosion_factory(position.position);
            m_registry.destroy(entity);
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
            if (!m_registry.valid(property.actualTarget) || property.actualTarget == entity) {
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

    void set_projectile_path() {
        A_Star a = A_Star(std::make_pair(tile_matrix_dimension.x, tile_matrix_dimension.y));
        for (auto &&[entity, property, position]: m_registry.view<ProjectileProperty, Position>().each()) {
            if (!m_registry.valid(property.actualTarget) || property.actualTarget == entity) {
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

    vec2 get_unit_scale(ShadedMesh &resource){
        vec2 scale = tile_size;
        float aspect_ratio = resource.mesh.original_size.x / resource.mesh.original_size.y;
        if(aspect_ratio>0) scale.y = tile_size.x/aspect_ratio;
        else scale.x = tile_size.y*aspect_ratio;
        return scale;
    }

    void create_projectile(entt::entity unit, UnitType unitType){
        auto entity = m_registry.create();
        ShadedMesh &resource = cache_resource("projectile");
        if (resource.effect.program.resource == 0){
            switch (unitType) {

                case UnitType::human_monitor:
                    RenderSystem::createSprite(resource, textures_path("human_tank_pro.png"), "textured");
                    m_registry.emplace<AllyProjectile>(entity);

                    break;
                case UnitType::human_archer:
                    RenderSystem::createSprite(resource, textures_path("human_archer_pro.png"), "textured");
                    m_registry.emplace<AllyProjectile>(entity);

                    break;
                case UnitType::human_healer:
                    RenderSystem::createSprite(resource, textures_path("human-healer_pro.png"), "textured");
                    m_registry.emplace<AllyProjectile>(entity);

                    break;

                case UnitType::ai_monitor:
                    RenderSystem::createSprite(resource, textures_path("ai_tank_pro.png"), "textured");
                    m_registry.emplace<EnemyProjectile>(entity);

                    break;
                case UnitType::ai_archer:
                    RenderSystem::createSprite(resource, textures_path("ai_archer_pro.png"), "textured");
                    m_registry.emplace<EnemyProjectile>(entity);

                    break;
                case UnitType::ai_healer:
                    RenderSystem::createSprite(resource, textures_path("ai_healer_pro.png"), "textured");
                    m_registry.emplace<EnemyProjectile>(entity);

                    break;
                case UnitType::human_terminator:
                    return;
                case UnitType::ai_terminator:
                    return;
                default:
                    assert(false);
                    break;
            }

        }

//        float random1 = ((rand() % 100) - 50) / 10.0f;
//        float random2 = ((rand() % 100) - 50) / 10.0f;
//        float rColor = 0.5f + ((rand() % 100) / 100.0f);
       if(m_registry.has<Projectiles>(unit)){
           return;
       }else{
           auto &projectile = m_registry.emplace<Projectiles>(unit);
           projectile.pro = entity;

           m_registry.emplace<ShadedMeshRef>(entity, resource);
           auto &motion = m_registry.emplace<Motion>(entity);
           motion.velocity = {0.f, 0.f} ;
           ProjectileProperty &property = m_registry.emplace<ProjectileProperty>(entity);
           property.unit_type = unitType;



           auto &position = m_registry.emplace<Position>(entity);
           auto &unit_position = m_registry.get<Position>(unit);
           position.position = unit_position.position;
           position.angle = 0.f;
           position.scale = get_unit_scale(resource);
           //position.scale = {35.f, 35.f};
       }



//        switch (unitType) {
//
//            case UnitType::human_monitor:
//
//                  property.damage = 5;
//
//                break;
//            case UnitType::human_archer:
//                RenderSystem::createSprite(resource, textures_path("human_archer_pro.png"), "textured");
//
//                break;
//            case UnitType::human_healer:
//                RenderSystem::createSprite(resource, textures_path("human-healer_pro.png"), "textured");
//
//                break;
//
//            case UnitType::ai_monitor:
//                RenderSystem::createSprite(resource, textures_path("ai_tank_pro.png"), "textured");
//
//                break;
//            case UnitType::ai_archer:
//                RenderSystem::createSprite(resource, textures_path("ai_archer_pro.png"), "textured");
//
//                break;
//            case UnitType::ai_healer:
//                RenderSystem::createSprite(resource, textures_path("ai_healer_pro.png"), "textured");
//
//                break;
//            default:
//                assert(false);
//                break;
//        }



    }

//    void init_projectile_bounding_box(entt::entity entity, UnitType unitType){
//        BoundingBox& boundingBox =m_registry.emplace<BoundingBox>(entity);
//        boundingBox.vertices = { vec2(-0.5, -0.5), vec2(0.5, -0.5), vec2(0.5, 0.5), vec2(-0.5, 0.5) };
//        switch (unitType) {
//            case UnitType::human_terminator:
//                boundingBox.vertices = { vec2(-0.1, -0.2), vec2(0.1, -0.2), vec2(0.2, 0.f), vec2(0.15, 0.3), vec2(-0.2, 0.3)};
//                return;
//            case UnitType::human_monitor:
//                return;
//            case UnitType::human_archer:
//                return ;
//            case UnitType::human_healer:
//                boundingBox.vertices = { vec2(-0.15, -0.25), vec2(0.1, -0.25), vec2(0.25, 0.1), vec2(0.25, 0.25), vec2(-0.15, 0.25) };
//                return ;
//            case UnitType::ai_terminator:
//                boundingBox.vertices = { vec2(-0.5, -0.1), vec2(-0.25, -0.5), vec2(0.3, -0.5), vec2(0.3, 0.5),
//                                         vec2(-0.25, 0.5), vec2(-0.5, 0.15) };
//                return ;
//            case UnitType::ai_monitor:
//                return ;
//            case UnitType::ai_archer:
//                boundingBox.vertices = { vec2(-0.5, -0.25), vec2(0.f, -0.5), vec2(0.5, -0.25),
//                                         vec2(0.5, 0.25), vec2(0.f, 0.5), vec2(-0.5, 0.25) };
//                return ;
//            case UnitType::ai_healer:
//                return;
//            default:
//                assert(false);
//                return;
//        }
//    }


    void update_state() {
        for (auto &&[entity, property]: m_registry.view<UnitProperty>().each()) {
            if (m_registry.valid(entity)) {
                if (within_attack_range(entity, property.actualTarget)) {
                    //stop at the proper attack range and then attack
                    property.path={};
                    create_projectile(entity,property.unit_type);

                    // eject projectiles
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
        set_projectile_path();
        update_state();
    }

}