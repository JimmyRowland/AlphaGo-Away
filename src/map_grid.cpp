// Header
#include "map_grid.hpp"
#include "render.hpp"
#include <iostream>
ECS::Entity Grid::createGrid(vec2 position, int grid_type, std::string texture_path, vec2 size)
{
    auto entity = ECS::Entity();
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
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Setting initial motion values
    Motion& motion = ECS::registry<Motion>.emplace(entity);
    motion.position = position;
    motion.angle = 0.f;
    // grids are not expected to move
    motion.velocity = { 0.f, 0.f };
    //motion.scale = resource.mesh.original_size * 0.5f;
    //motion.scale = vec2({ 0.7f, 0.7f }) * static_cast<vec2>(resource.texture.size);
    motion.scale = size;
    //motion.scale.x *= -1; // point front to the right
    
    // can delete this id field if not needed when constructing the whole map
    int type = grid_type;
    //std::cout << motion.position.x << motion.position.y << std::endl;
    // Create and (empty) Salmon component to be able to refer to all turtles
    ECS::registry<Grid>.emplace(entity);

    return entity;
}
