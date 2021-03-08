//// Header
//#include "ScreenComponent.hpp"
//#include "render.hpp"
//#include <iostream>
//ECS::Entity ScreenComponent::createScreenComponent(vec2 position, std::string texture_path, vec2 size, float depth, COMPONENT_TYPE c_type)
//{
//    auto entity = ECS::Entity();
//    int id = 10;
//
//    std::string key = texture_path;
//    ShadedMesh& resource = cache_resource(key);
//    if (resource.effect.program.resource == 0)
//    {
//        resource = ShadedMesh();
//        RenderSystem::createSprite(resource, textures_path(texture_path), "textured");
//    }
//    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
//    ECS::registry<ShadedMeshRef>.emplace(entity, resource);
//    ShadedMeshRef& mesh_ref = ECS::registry<ShadedMeshRef>.get(entity);
//    mesh_ref.depth = depth;
////    std::cout << "image: " << texture_path << std::endl;
////    std::cout << "depth: " << mesh_ref.depth << std::endl;
//
//    // Setting initial motion values
//    Motion& motion = ECS::registry<Motion>.emplace(entity);
//    motion.position = position;
//    motion.angle = 0.f;
//
//    motion.velocity = { 0.f, 0.f };
//    motion.scale = size;
//
//    ECS::registry<ScreenComponent>.emplace(entity);
//
//    switch (c_type) {
//        case BUTTON:
//            ECS::registry<ButtonComponent>.emplace(entity);
//            break;
//
//        default:
//            break;
//    }
//
//    return entity;
//}
//
