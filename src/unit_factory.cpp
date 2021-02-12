// Header
#include "unit_factory.hpp"
#include "render.hpp"

UnitFactory::UnitFactory(LevelStateSystem &levelState) : level_state(levelState) {
}

ECS::Entity UnitFactory::create_unit(vec2 position) {
    auto entity = ECS::Entity();


    std::string key = "unit";
    ShadedMesh& resource = cache_resource(key);
    if (resource.mesh.vertices.size() == 0)
    {
        resource.mesh.loadFromOBJFile(mesh_path("unit1.obj"));
        RenderSystem::createColoredMesh(resource, "unit");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Setting initial motion values
    Motion& motion = ECS::registry<Motion>.emplace(entity);
    motion.position = position;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = resource.mesh.original_size * 150.f;
    ECS::registry<Unit>.emplace(entity);
    Property& property = ECS::registry<Property>.emplace(entity);



    if(!level_state.update_gold()){

        property.isEnemy = 1;
    }else{
        property.isEnemy = 0;
    }

    return entity;

}
