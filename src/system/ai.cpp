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
            if(projectile.pro != entt::null && m_registry.valid(projectile.pro)){
                m_registry.destroy(projectile.pro);}
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

    ShadedMesh& create_projectile_mesh(std::string screen_texture_path, std::string shader = "textured"){
        float random1 = ((rand() % 100) - 50) / 10.0f;
        std::string rand = std::to_string(random1);
        std::string key = screen_texture_path ;
        key.append(rand);
        ShadedMesh &resource = cache_resource(key);
        RenderSystem::createSprite(resource, textures_path(screen_texture_path), "textured");
        return resource;
    }

    void create_projectile(entt::entity unit, UnitType unitType){
        auto entity = m_registry.create();

        //auto &position = m_registry.emplace<Position>(entity);
        //ShadedMesh &resource = cache_resource("projectile");
        //std::string key = "human_monitor";

//        if (resource.effect.program.resource == 0){
            switch (unitType) {
                case UnitType::human_monitor:

                    m_registry.emplace<ShadedMeshRef>(entity, create_projectile_mesh("human_tank_pro.png", "textured"));

                    //RenderSystem::createSprite(resource, textures_path("human_tank_pro.png"), "textured");
                    m_registry.emplace<AllyProjectile>(entity);

                    break;
                case UnitType::human_archer:
                    m_registry.emplace<ShadedMeshRef>(entity, create_projectile_mesh("human_archer_pro.png", "textured"));

                    //RenderSystem::createSprite(resource, textures_path("human_archer_pro.png"), "textured");
                    m_registry.emplace<AllyProjectile>(entity);


                    break;
                case UnitType::human_healer:
                    m_registry.emplace<ShadedMeshRef>(entity, create_projectile_mesh("human-healer_pro.png", "textured"));

                    //RenderSystem::createSprite(resource, textures_path("human-healer_pro.png"), "textured");
                    m_registry.emplace<AllyProjectile>(entity);

                    break;

                case UnitType::ai_monitor:
                    m_registry.emplace<ShadedMeshRef>(entity, create_projectile_mesh("ai_tank_pro.png", "textured"));

                    // RenderSystem::createSprite(resource, textures_path("ai_tank_pro.png"), "textured");
                    m_registry.emplace<EnemyProjectile>(entity);

                    break;
                case UnitType::ai_archer:
                    m_registry.emplace<ShadedMeshRef>(entity, create_projectile_mesh("ai_archer_pro.png", "textured"));

                    //RenderSystem::createSprite(resource, textures_path("ai_archer_pro.png"), "textured");
                    m_registry.emplace<EnemyProjectile>(entity);

                    break;
                case UnitType::ai_healer:
                    m_registry.emplace<ShadedMeshRef>(entity, create_projectile_mesh("ai_healer_pro.png", "textured"));

                    //RenderSystem::createSprite(resource, textures_path("ai_healer_pro.png"), "textured");
                    m_registry.emplace<EnemyProjectile>(entity);
                    std::cout << "draw" << std::endl;
                    break;
                case UnitType::human_terminator:
                    return;
                case UnitType::ai_terminator:
                    return;
                default:
                    assert(false);
                    break;
            }

//        }

        //;
        //float random2 = ((rand() % 100) - 50) / 10.0f;
       if(m_registry.has<Projectiles>(unit)){
           return;
       }else{
           auto &projectile = m_registry.emplace<Projectiles>(unit);
           projectile.pro = entity;

           //m_registry.emplace<ShadedMeshRef>(entity, resource);
           auto &motion = m_registry.emplace<Motion>(entity);
           motion.velocity = {0,0} ;
           ProjectileProperty &property = m_registry.emplace<ProjectileProperty>(entity);
           property.unit_type = unitType;



           auto &position = m_registry.emplace<Position>(entity);
           auto &unit_position = m_registry.get<Position>(unit);
           position.position = unit_position.position;
           position.angle = 0.f;
           //position.scale = {get_unit_scale(resource).x/3, get_unit_scale(resource).y/3};
           position.scale = {40.f, 40.f};
       }


    }


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
                    if(m_registry.has<Projectiles>(entity)) {
                        auto &projectile = m_registry.get<Projectiles>(entity);
                        if (projectile.pro != entt::null && m_registry.valid(projectile.pro)) {
                            m_registry.destroy(projectile.pro);
                        }
                    }
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