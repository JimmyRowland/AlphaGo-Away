//
// Created by grumpy on 2021-03-13.
//

#include "unit.hpp"
#include <iostream>
namespace {
    nlohmann::json unit_meshes;
}
void init_unit_resources(){
    load_json("textures.json", unit_meshes);
}

ShadedMesh &create_unit_mesh_resource(UnitType unitType, std::string unit_state) {
    nlohmann::json stand_mesh_json = unit_meshes[get_unit_mesh_key[unitType]][unit_state];
    std::string tile_texture_path = stand_mesh_json["texture"];
    std::string key = tile_texture_path;
    ShadedMesh &resource = cache_resource(key);
    resource.number_of_frames = stand_mesh_json["number_of_frame"];
    if (resource.effect.program.resource == 0)
        RenderSystem::createSprite(resource, textures_path(tile_texture_path), stand_mesh_json["shader"]);
    return resource;
}


void remove_all_unit_state_components(entt::entity entity){
    m_registry.remove_if_exists<Stand>(entity);
    m_registry.remove_if_exists<Attack>(entity);
    m_registry.remove_if_exists<Die>(entity);
    m_registry.remove_if_exists<Walk>(entity);
};
void unit_attack(entt::entity entity, UnitType unitType){
    if(m_registry.valid(entity) && !m_registry.has<Attack>(entity)){
        ShadedMesh &resource = create_unit_mesh_resource(unitType, "attack");
        remove_all_unit_state_components(entity);
        m_registry.emplace<Attack>(entity);
        m_registry.emplace_or_replace<ShadedMeshRef>(entity,resource);
    }
};
void unit_stand(entt::entity entity, UnitType unitType){
    if(m_registry.valid(entity) && !m_registry.has<Stand>(entity)) {
        ShadedMesh &resource = create_unit_mesh_resource(unitType, "stand");
        remove_all_unit_state_components(entity);
        m_registry.emplace<Stand>(entity);
        m_registry.emplace_or_replace<ShadedMeshRef>(entity, resource);
    }
};
void unit_walk(entt::entity entity, UnitType unitType){
    if(m_registry.valid(entity) &&!m_registry.has<Walk>(entity)) {
        ShadedMesh &resource = create_unit_mesh_resource(unitType, "walk");
        remove_all_unit_state_components(entity);
        m_registry.emplace<Walk>(entity);
        m_registry.emplace_or_replace<ShadedMeshRef>(entity, resource);
    }
};
void unit_die(entt::entity entity, UnitType unitType){
    if(m_registry.valid(entity) &&!m_registry.has<Die>(entity)) {
        ShadedMesh &resource = create_unit_mesh_resource(unitType, "die");
        remove_all_unit_state_components(entity);
        m_registry.emplace<Die>(entity);
        m_registry.emplace_or_replace<ShadedMeshRef>(entity, resource);
    }
};
