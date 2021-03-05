#include "factories.hpp"

entt::entity ground_unit_factory(vec2 pos, bool should_place_enemy){
    auto entity = m_registry.create();
    std::string key = "unit";
    ShadedMesh& resource = cache_resource(key);
    if (resource.mesh.vertices.size() == 0)
    {
            resource.mesh.loadFromOBJFile(mesh_path("unit1.obj"));
            RenderSystem::createColoredMesh(resource, key);
    };

    m_registry.emplace<ShadedMeshRef>(entity, resource);
    auto& motion = m_registry.emplace<Motion>(entity);
    auto& position = m_registry.emplace<Position>(entity);


    position.position = pos;
    position.angle = 0.f;
    motion.velocity = { 10.f, 10.f };
    position.scale = resource.mesh.original_size * 150.f;
    m_registry.emplace<GroundUnit>(entity);

    UnitProperty& property = m_registry.emplace<UnitProperty>(entity);

    if(should_place_enemy){
        m_registry.emplace<Enemy>(entity);
    }else{
        m_registry.emplace<Ally>(entity);
    }
    return entity;
};
std::string get_tile_path(TileType tileType) {
    switch (tileType) {
        case TileType::basic: return "basic_grid.png";
        case TileType::forest: return "tree_grid.png";
        case TileType::water: return "river_grid.png";
        default:
            assert(false);
            return {};
    }
}

entt::entity tile_factory(vec2 pos, TileType tileType){
    auto entity = m_registry.create();
    std::string tile_texture_path = get_tile_path(tileType);
    std::string key = tile_texture_path;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
        RenderSystem::createSprite(resource, textures_path(tile_texture_path), "textured");
    m_registry.emplace<ShadedMeshRef>(entity, resource);
    auto& position = m_registry.emplace<Position>(entity);
    position.position = pos;
    position.angle = 0.f;
    position.scale = tile_size;
    m_registry.emplace<Tile>(entity);
    switch (tileType) {
        case TileType::basic: m_registry.emplace<Basic>(entity); break;
        case TileType::forest: m_registry.emplace<Forest>(entity); break;
        case TileType::water:  m_registry.emplace<Water>(entity); break;
        default:
            assert(false);
    }
    return entity;
};
