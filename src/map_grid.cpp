// Header
#include "map_grid.hpp"
#include "render.hpp"
#include <iostream>

ECS::Entity Grid::createGrid(vec2 position, int grid_type, std::string texture_path, vec2 size, std::pair<int, int> gridPos)
{
    auto entity = ECS::Entity();
    int type;

    std::string key = "grid";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(texture_path), "textured");
    }
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    ShadedMeshRef& mesh_ref = ECS::registry<ShadedMeshRef>.get(entity);
    mesh_ref.depth = 800;

    // Setting initial motion values
    Motion& motion = ECS::registry<Motion>.emplace(entity);
    motion.position = position;
    motion.angle = 0.f;
    // grids are not expected to move
    motion.velocity = { 0.f, 0.f };
    //motion.scale = resource.mesh.original_size * 0.5f;
    //motion.scale = vec2({ 0.7f, 0.7f }) * static_cast<vec2>(resource.texture.size);
    motion.scale = size;
    motion.gridPos = gridPos;
    //motion.scale.x *= -1; // point front to the right

    GridProperty& gProperty = ECS::registry<GridProperty>.emplace(entity);
    gProperty.type = grid_type;

    // can delete this id field if not needed when constructing the whole map
    //std::cout << motion.position.x << motion.position.y << std::endl;
    // Create and (empty) Salmon component to be able to refer to all turtles
    ECS::registry<Grid>.emplace(entity);

    return entity;
}
