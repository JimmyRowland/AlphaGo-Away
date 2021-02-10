// Header
#include "map_grid.hpp"
#include "system/render.hpp"
#include <iostream>
entt::entity Grid::createGrid(vec2 pos, int in_id, std::string texture_path)
{
    auto entity = m_registry.create();
    int id = 10;

    std::string key = "grid";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        // TODO: I think we can regard grids as sprite? If not, we might need to create another method similar to createSprite() in render_init.cpp
        RenderSystem::createSprite(resource, textures_path(texture_path), "textured");
    }
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    m_registry.emplace<ShadedMeshRef>(entity, resource);

    // Setting initial motion values
    Motion& motion = m_registry.emplace<Motion>(entity);
    motion.position = pos;
    motion.angle = 0.f;
    // grids are not expected to move
    motion.velocity = { 0.f, 0.f };
    motion.scale = resource.mesh.original_size * 0.5f;
    motion.scale.x *= -1; // point front to the right
    
    // can delete this id field if not needed when constructing the whole map
    int type = in_id;

    // Create and (empty) Salmon component to be able to refer to all turtles
    m_registry.emplace<Grid>(entity);

    return entity;
}
