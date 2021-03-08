//// Header
//#include "enemy.hpp"
//#include "render.hpp"
//#include <iostream>
//ECS::Entity Enemy::createEnemy(vec2 position, std::string texture_path, vec2 size, vec2 coor)
//{
//    auto entity = ECS::Entity();
//    int id = 10;
//
//    std::string key = "enemy";
//    ShadedMesh& resource = cache_resource(key);
//    if (resource.effect.program.resource == 0)
//    {
//        resource = ShadedMesh();
//        RenderSystem::createSprite(resource, textures_path(texture_path), "textured");
//    }
//    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
//    ECS::registry<ShadedMeshRef>.emplace(entity, resource);
//
//    // Setting initial motion values
//    Motion& motion = ECS::registry<Motion>.emplace(entity);
//    motion.position = position;
//    motion.angle = 0.f;
//    motion.velocity = { -50.f, 0.f };
//    motion.scale = size;
//
//
//    // can delete this id field if not needed when constructing the whole map
//    //int type = ai_type;
//
//    ECS::registry<Enemy>.emplace(entity);
//
//    return entity;
//}
//
