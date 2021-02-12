// Header
#include "unit.hpp"
#include "render.hpp"

ECS::Entity Unit::createUnit(vec2 position)
{
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
    ECS::registry<Select>.emplace(entity);

	motion.position = position;
	motion.angle = 0.f;
	motion.velocity = { 50.f, 0.f };
	motion.scale = resource.mesh.original_size * 150.f;
	motion.scale.x *= -1; // point front to the right

	// Create and (empty) Salmon component to be able to refer to all turtles
	ECS::registry<Unit>.emplace(entity);

	return entity;
}