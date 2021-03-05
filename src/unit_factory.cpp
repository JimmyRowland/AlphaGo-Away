// Header
#include "unit_factory.hpp"
#include "render.hpp"
#include <iostream>

UnitFactory::UnitFactory(LevelStateSystem &levelState) : level_state(levelState) {
}

ECS::Entity UnitFactory::create_unit(vec2 position, unsigned int unit_type) {
    auto entity = ECS::Entity();
    std::string key = "unit";
    if(unit_type==TERMINATOR){
        key = "unit";
    }if(unit_type==MONITOR){
        key = "monitor";
    }
    ShadedMesh& resource = cache_resource(key);
    if (resource.mesh.vertices.size() == 0)
    {
        if(unit_type==TERMINATOR){
            resource.mesh.loadFromOBJFile(mesh_path("unit1.obj"));
            RenderSystem::createColoredMesh(resource, key);

        }
        if(unit_type==MONITOR){
            resource = ShadedMesh();
            RenderSystem::createSprite(resource, textures_path("ai_default.png"), "textured");
//            resource.mesh.loadFromOBJFile(mesh_path("unit1.obj"));
//            RenderSystem::createColoredMesh(resource, key);
        }

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

    if(should_place_enemy){
        property.isEnemy=1;
    }else{
        property.isEnemy=0;
        level_state.update_health();
    }
    if(unit_type==MONITOR){
        motion.scale = vec2(gridWidth,gridHeight);
        property.hp = 1000;
    }

    Transform transform;
    transform.translate(motion.position);
    transform.rotate(motion.angle);
    transform.scale(motion.scale);

    if (key == "monitor" ) {
        BoundingBox& boundingBox = ECS::registry<BoundingBox>.emplace(entity);
        boundingBox.vertices = { vec2(0.f, resource.mesh.original_size.y / 4), vec2(resource.mesh.original_size.x / 2, 0.f), vec2(resource.mesh.original_size.x, resource.mesh.original_size.y / 4),
                                vec2(resource.mesh.original_size.x, 3* resource.mesh.original_size.y / 4), vec2(resource.mesh.original_size.x / 2, resource.mesh.original_size.y), vec2(0.f, 3 * resource.mesh.original_size.y / 4) };
        for (int i = 0; i < boundingBox.vertices.size(); i++) {
            boundingBox.vertices[i] = transform.mat * vec3(boundingBox.vertices[i].x, boundingBox.vertices[i].y, 1.f);

            //std::cout << boundingBox.vertices[i].x << "," << boundingBox.vertices[i].y << std::endl;
        }
        
    }
    else if (key == "unit") {
        BoundingBox& boundingBox = ECS::registry<BoundingBox>.emplace(entity);
        auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;
        float top = 0;
        float bottom = 0;
        float right = 0;
        float left = 0;
        for (ColoredVertex pos : texmesh.mesh.vertices) {
            auto position = pos.position;
            //std::cout << position.x << "," << position.y << std::endl;
            position = transform.mat * vec3(position.x, position.y, 1.f);
            if (position.x >= right) {
                right = position.x;
            }
            if (position.x <= left) {
                left = position.x;
            }
            if (position.y >= bottom) {
                bottom = position.y;
            }
            if (position.y <= top) {
                top = position.y;
            }
        }
        //std::cout << top << "," << right << "," << bottom << "," << left << std::endl;
        boundingBox.vertices = { vec2(left, top), vec2(right, top), vec2(right, bottom), vec2(left, bottom) };
    }

    return entity;

}
void UnitFactory::on_key_click(int key, int action) {
    if (action == GLFW_PRESS && key == GLFW_KEY_E)
    {
        should_place_enemy = SHOULD_PLACE_ENEMY;
    }
    if (action == GLFW_PRESS && key == GLFW_KEY_H)
    {
        should_place_enemy = SHOULD_PLACE_HUMAN;
    }
}

void UnitFactory::setGridWidth(float gridWidth) {
    UnitFactory::gridWidth = gridWidth;
}

void UnitFactory::setGridHeight(float gridHeight) {
    UnitFactory::gridHeight = gridHeight;
}

LevelStateSystem& UnitFactory::getLevelState() const {
    return level_state;
}
