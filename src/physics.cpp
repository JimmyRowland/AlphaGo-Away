// internal
#include "debug.hpp"
#include "physics.hpp"
#include "tiny_ecs.hpp"
#include <iostream>

// Returns the local bounding coordinates scaled by the current size of the entity 
vec2 get_bounding_box(const Motion& motion)
{
	// fabs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You don't
// need to try to use this technique.

bool satHelper(const ECS::Entity& e1, const ECS::Entity& e2) {
    BoundingBox bb1 = ECS::registry<BoundingBox>.get(e1); // bounding box for first entity
    BoundingBox bb2 = ECS::registry<BoundingBox>.get(e2); // bounding box for second entity
    if (bb1.vertices.size() <= 0 || bb2.vertices.size() <= 0) { // all unit entities should have a bounding box
        return false; 
    }
    for (int i = 0; i < bb1.transformed_vertices.size(); i++) {
        auto current = bb1.transformed_vertices[i];
        auto next = bb1.transformed_vertices[(i + 1) % bb1.transformed_vertices.size()];
        auto edge = next - current;
        vec2 axis = vec2(-edge.y, edge.x);

        auto e1MaxProj = NULL;
        auto e2MaxProj = NULL;
        auto e1MinProj = NULL;
        auto e2MinProj = NULL;
        for (vec2 v : bb1.transformed_vertices) { // project entity 1's bb vertices onto the axis
            auto projection = dot(axis, v);
            if (e1MaxProj == NULL || projection > e1MaxProj) {
                e1MaxProj = projection;
            }
            if (e1MinProj == NULL || projection < e1MinProj) {
                e1MinProj = projection;
            }
        }
        for (vec2 v : bb2.transformed_vertices) { // project entity 2's bb vertices onto the axis
            auto projection = dot(axis, v);
            if (e2MaxProj == NULL || projection > e2MaxProj) {
                e2MaxProj = projection;
            }
            if (e2MinProj == NULL || projection < e2MinProj) {
                e2MinProj = projection;
            }
        }
        if (e1MaxProj < e2MinProj || e1MinProj > e2MaxProj) {
            return false;
        }
    }
    return true;
}

//bool collides(const Motion& motion1, const Motion& motion2, const Property& prop1, const Property& prop2)
bool collides(const ECS::Entity& e1, const ECS::Entity& e2)
{
    bool e1Check = satHelper(e1, e2);
    bool e2Check = satHelper(e2, e1);
    return (e1Check && e2Check);

}

void PhysicsSystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
    if(should_pause) return;

    // Check for collisions between all moving entities
    // for (auto [i, motion_i] : enumerate(motion_container.components)) // in c++ 17 we will be able to do this instead of the next three lines
    for (auto entity: ECS::view<Property, Motion>())
    {
        Motion& motion = ECS::registry<Motion>.get(entity);
        if(ECS::registry<Property>.has(entity)){
            Property& property = ECS::registry<Property>.get(entity);
            if(ECS::registry<Motion>.has(property.target)){
                Motion& motion_j = ECS::registry<Motion>.get(property.target);
                vec2 acceleration = (motion_j.position-motion.position)*vec2(0.1,0.1);
                motion.velocity += acceleration;
                if(pow(motion.velocity.x,2)+pow(motion.velocity.y,2) >1600){
                    float angle = atan2(motion.velocity.y, motion.velocity.x);
                    motion.velocity.x = cos(angle)*40;
                    motion.velocity.y = sin(angle)*40;
                }
            }
        }
        if(motion.velocity.x > 0 && motion.scale.x > 0){
            motion.scale.x *= -1;
        }else if(motion.velocity.x < 0 && motion.scale.x < 0){
            motion.scale.x *= -1;
        }

        float step_seconds = 1.0f * (elapsed_ms / 1000.f);
        motion.velocity = vec2(get_velocity_after_drag(motion.velocity.x),get_velocity_after_drag(motion.velocity.y));
        motion.position+=motion.velocity*step_seconds;


    }
    
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	(void)window_size_in_game_units;

	// Visualization for debugging the position and scale of objects
	if (DebugSystem::in_debug_mode)
	{
		for (auto& bb : ECS::registry<BoundingBox>.components)
		{
			// draw a cross at the position of all objects
            for (auto& vertices : bb.transformed_vertices) {
                auto dotSize = vec2(5.f, 5.f);
                DebugSystem::createLine(vertices, dotSize);
            }
		}
	}

	// for (auto [i, motion_i] : enumerate(motion_container.components)) // in c++ 17 we will be able to do this instead of the next three lines
    auto& motion_container = ECS::registry<Motion>;
	for (unsigned int i=0; i<motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		ECS::Entity entity_i = motion_container.entities[i];
//		Property& property_i = ECS::registry<Property>.get(entity_i);
		for (unsigned int j=i+1; j<motion_container.components.size(); j++)
		{
			Motion& motion_j = motion_container.components[j];
			ECS::Entity entity_j = motion_container.entities[j];

            if(ECS::registry<Property>.has(entity_i) && ECS::registry<Property>.has(entity_j)){
                Property& property_i = ECS::registry<Property>.get(entity_i);
                Property& property_j = ECS::registry<Property>.get(entity_j);
                if(property_i.isEnemy!=property_j.isEnemy){
                    if(!ECS::registry<Motion>.has(property_i.target)){
                        property_i.target = entity_j;
                    }
                    if(!ECS::registry<Motion>.has(property_j.target)){
                        property_j.target = entity_i;
                    }
                }

                //if (collides(motion_i, motion_j, property_i, property_j)){
                if (collides(entity_i, entity_j)){
                    // Create a collision event
                    // Note, we are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity, hence, emplace_with_duplicates
                    ECS::registry<Collision>.emplace_with_duplicates(entity_i, entity_j);
                    ECS::registry<Collision>.emplace_with_duplicates(entity_j, entity_i);
                    for(auto& observer: collision_observers){
                        observer(entity_i, entity_j);
                    }
                    float step_seconds = 1.0f * (elapsed_ms / 1000.f);
                    vec2 direction = motion_j.position-motion_i.position;
                    if(direction.x == 0 && direction.y ==0){
                        direction.x = 20;
                    }
                    motion_j.position+=direction*step_seconds*10.f;
                    motion_i.position+=direction*step_seconds*-10.f;
                }
            }
		}
	}

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Add any update to physic system here.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

void PhysicsSystem::on_key_click(int key, int action) {
    if (action == GLFW_PRESS && key == GLFW_KEY_P)
    {
        should_pause = !should_pause;
    }
}

PhysicsSystem::Collision::Collision(ECS::Entity& other)
{
	this->other = other;
}
