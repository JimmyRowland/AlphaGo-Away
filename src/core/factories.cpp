#include "factories.hpp"

namespace {
    std::string get_tile_path(TileType tileType) {
        switch (tileType) {
            case TileType::basic:
                return "basic_grid.png";
            case TileType::forest:
                return "tree_grid.png";
            case TileType::water:
                return "river_grid.png";
            default:
                assert(false);
                return {};
        }
    }

    ShadedMesh &create_tile_mesh_resource(TileType tileType) {
        std::string tile_texture_path = get_tile_path(tileType);
        std::string key = tile_texture_path;
        ShadedMesh &resource = cache_resource(key);
        if (resource.effect.program.resource == 0)
            RenderSystem::createSprite(resource, textures_path(tile_texture_path), "textured");
        return resource;
    }

    std::string get_unit_path(UnitType unitType) {
        switch (unitType) {
            case UnitType::human_terminator: return "close-001.png";
            case UnitType::human_monitor:return "tank-001.png";
            case UnitType::human_archer:return "walking_unit.png";
            case UnitType::human_healer:return "recover-001.png";
            case UnitType::ai_terminator:return "ai_short-walk.png";
            case UnitType::ai_monitor:return "ai_tank-2.png";
            case UnitType::ai_archer:return "ai_long-d.png";
            case UnitType::ai_healer:return "ai_recover.png";
            default:
                assert(false);
                return {};
        }
    }

    std::string get_texture_type(UnitType unitType) {
        switch (unitType) {
            case UnitType::human_terminator: return "textured";
            case UnitType::human_monitor:return "textured";
            case UnitType::human_archer:return "anime_textured";
            case UnitType::human_healer:return "textured";
            case UnitType::ai_terminator:return "walking_robot_textured";
            case UnitType::ai_monitor:return "textured";
            case UnitType::ai_archer:return "textured";
            case UnitType::ai_healer:return "textured";

            default:
                assert(false);
                return {};
        }
    }

    ShadedMesh &create_unit_mesh_resource(UnitType unitType) {
        std::string tile_texture_path = get_unit_path(unitType);
        std::string key = tile_texture_path;
        ShadedMesh &resource = cache_resource(key);
        if (resource.effect.program.resource == 0)
            RenderSystem::createSprite(resource, textures_path(tile_texture_path), get_texture_type(unitType));
        return resource;
    }
    
    vec2 get_unit_scale(ShadedMesh &resource){
        vec2 scale = tile_size;
        float aspect_ratio = resource.mesh.original_size.x / resource.mesh.original_size.y;
        if(aspect_ratio>0) scale.y = tile_size.x/aspect_ratio;
        else scale.x = tile_size.y*aspect_ratio;
        return scale;
    }

    void init_unit_flag_components(entt::entity entity, UnitType unitType){
        switch (unitType) {
            case UnitType::human_terminator:
                m_registry.emplace<Terminator>(entity);
                m_registry.emplace<Ally>(entity);
                return;
            case UnitType::human_monitor:
                m_registry.emplace<Monitor>(entity);
                m_registry.emplace<Ally>(entity);
                return;
            case UnitType::human_archer:
                m_registry.emplace<Archer>(entity);
                m_registry.emplace<Ally>(entity);
                return ;
            case UnitType::human_healer:
                m_registry.emplace<Healer>(entity);
                m_registry.emplace<Ally>(entity);
                return ;
            case UnitType::ai_terminator:
                m_registry.emplace<Terminator>(entity);
                m_registry.emplace<Enemy>(entity);
                return ;
            case UnitType::ai_monitor:
                m_registry.emplace<Monitor>(entity);
                m_registry.emplace<Enemy>(entity);
                return ;
            case UnitType::ai_archer:
                m_registry.emplace<Archer>(entity);
                m_registry.emplace<Enemy>(entity);
                return ;
            case UnitType::ai_healer:
                m_registry.emplace<Healer>(entity);
                m_registry.emplace<Enemy>(entity);
                return;
            default:
                assert(false);
                return;
        }
    }

    void init_unit_bounding_box(entt::entity entity, UnitType unitType){
        BoundingBox& boundingBox =m_registry.emplace<BoundingBox>(entity);
        boundingBox.vertices = { vec2(-0.5, -0.5), vec2(0.5, -0.5), vec2(0.5, 0.5), vec2(-0.5, 0.5) };
        switch (unitType) {
            case UnitType::human_terminator:
                boundingBox.vertices = { vec2(-0.1, -0.2), vec2(0.1, -0.2), vec2(0.2, 0.f), vec2(0.15, 0.3), vec2(-0.2, 0.3)};
                return;
            case UnitType::human_monitor:
                return;
            case UnitType::human_archer:
                return ;
            case UnitType::human_healer:
                boundingBox.vertices = { vec2(-0.15, -0.25), vec2(0.1, -0.25), vec2(0.25, 0.1), vec2(0.25, 0.25), vec2(-0.15, 0.25) };
                return ;
            case UnitType::ai_terminator:
                boundingBox.vertices = { vec2(-0.5, -0.1), vec2(-0.25, -0.5), vec2(0.3, -0.5), vec2(0.3, 0.5),
                                         vec2(-0.25, 0.5), vec2(-0.5, 0.15) };
                return ;
            case UnitType::ai_monitor:
                return ;
            case UnitType::ai_archer:
                boundingBox.vertices = { vec2(-0.5, -0.25), vec2(0.f, -0.5), vec2(0.5, -0.25),
                                         vec2(0.5, 0.25), vec2(0.f, 0.5), vec2(-0.5, 0.25) };
                return ;
            case UnitType::ai_healer:
                return;
            default:
                assert(false);
                return;
        }
    }

    void init_unit(entt::entity entity, ShadedMesh &resource, vec2 pos, UnitType unitType){
        auto &motion = m_registry.emplace<Motion>(entity);
        motion.velocity = {0.f, 0.f};
        auto &position = m_registry.emplace<Position>(entity);
        position.position = pos;
        position.angle = 0.f;
        position.scale = get_unit_scale(resource);
        UnitProperty &property = m_registry.emplace<UnitProperty>(entity);
    }
}

entt::entity unit_factory(vec2 pos, UnitType unitType) {
    auto entity = m_registry.create();
    ShadedMesh &resource = create_unit_mesh_resource(unitType);
    m_registry.emplace<ShadedMeshRef>(entity, resource);
    init_unit_flag_components(entity, unitType);
    init_unit_bounding_box(entity, unitType);
    init_unit(entity, resource, pos, unitType);
    return entity;
};


entt::entity tile_factory(vec2 pos, TileType tileType) {
    auto entity = m_registry.create();
    ShadedMesh &resource = create_tile_mesh_resource(tileType);
    m_registry.emplace<ShadedMeshRef>(entity, resource);
    auto &position = m_registry.emplace<Position>(entity);
    position.position = pos;
    position.angle = 0.f;
    position.scale = tile_size;
    auto &tile_comp = m_registry.emplace<Tile>(entity);
    tile_comp.type = tileType;
    return entity;
};

void swap_tile_texture(entt::entity entity, TileType tileType) {
    auto &tile_comp = m_registry.get<Tile>(entity);
    if (tile_comp.type != tileType) {
        tile_comp.type = tileType;
        ShadedMesh &resource = create_tile_mesh_resource(tileType);
        m_registry.replace<ShadedMeshRef>(entity, resource);
    }
};

GLuint get_tile_texture_id(TileType tileType) {
    ShadedMesh &resource = create_tile_mesh_resource(tileType);
    return resource.texture.texture_id;
};