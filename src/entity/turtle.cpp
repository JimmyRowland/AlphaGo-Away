// Header
#include "turtle.hpp"
#include "system/render.hpp"

entt::entity Turtle::createTurtle(vec2 position)
{
	auto entity = m_registry.create();

	// Create rendering primitives
	std::string key = "turtle";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0)
		RenderSystem::createSprite(resource, textures_path("turtle.png"), "textured");

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	m_registry.emplace<ShadedMeshRef>(entity, resource);

	// Initialize the motion
	auto& motion = m_registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 100.f, 0.f }; // 200
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -0.4f, 0.4f }) * static_cast<vec2>(resource.texture.size);

	// Create and (empty) Turtle component to be able to refer to all turtles
	m_registry.emplace<Turtle>(entity);

	return entity;
}
