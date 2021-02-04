// Header
#include "Army.hpp"
#include "render.hpp"



ECS::Entity Army::createArmy(vec2 position)
{
	auto entity = ECS::Entity();

	std::string key = "army";

#pragma region ShadedComponent
	// TODO: Load army picture 
	ShadedMesh& resource = cache_resource(key);
	if (resource.mesh.vertices.size() == 0)
	{
		resource.mesh.loadFromOBJFile(mesh_path("salmon.obj"));
		RenderSystem::createColoredMesh(resource, "salmon");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ECS::registry<ShadedMeshRef>.emplace(entity, resource);
#pragma endregion

#pragma region Motion Component
	// Setting initial motion values
	Motion& motion = ECS::registry<Motion>.emplace(entity);
	motion.position = position;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = resource.mesh.original_size * 150.f;
	motion.scale.x *= -1; // point front to the right
	motion.cur = false;
#pragma endregion

#pragma region Button Component
	Button& button = ECS::registry<Button>.emplace(entity);
#pragma endregion

	// Create and (empty) Salmon component to be able to refer to all turtles
	ECS::registry<Army>.emplace(entity);
	return entity;
}


ECS::Entity SwordMan::createSwordMan(vec2 position)
{
	auto entity = ECS::Entity();

	std::string key = "sword";

#pragma region ShadedComponent
	// TODO: Load army picture 
	ShadedMesh& resource = cache_resource(key);
	if (resource.mesh.vertices.size() == 0)
	{
		resource.mesh.loadFromOBJFile(mesh_path("salmon.obj"));
		RenderSystem::createColoredMesh(resource, "salmon");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ECS::registry<ShadedMeshRef>.emplace(entity, resource);
#pragma endregion

#pragma region Motion Component
	// Setting initial motion values
	Motion& motion = ECS::registry<Motion>.emplace(entity);
	motion.position = position;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = resource.mesh.original_size * 150.f;
	motion.scale.x *= -1; // point front to the right
	motion.cur = false;
#pragma endregion

	Property& property = ECS::registry<Property>.emplace(entity);
	//ReSet(int hp, int atkR, int mov, int dmg, float dmgr, int blk)
	property = { 5, 5, 5, 5, 5, 5 }; // just for testing

#pragma region 
	Button& button = ECS::registry<Button>.emplace(entity);
#pragma endregion

	// Put 
	ECS::registry<Army>.emplace(entity);
	ECS::registry<SwordMan>.emplace(entity);
	return entity;
}