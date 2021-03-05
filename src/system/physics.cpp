// internal
#include "physics.hpp"

#include "logger/debug.hpp"

namespace {
    vec2 get_bounding_box(const Position& position)
    {
        return { abs(position.scale.x), abs(position.scale.y) };
    }

    bool collides(const Position &position1, const Position &position2)
    {
        auto dp = position1.position - position2.position;
        float dist_squared = dot(dp,dp);
        float other_r = std::sqrt(std::pow(get_bounding_box(position1).x / 2.0f, 2.f) + std::pow(get_bounding_box(position1).y / 2.0f, 2.f));
        float my_r = std::sqrt(std::pow(get_bounding_box(position2).x / 2.0f, 2.f) + std::pow(get_bounding_box(position2).y / 2.0f, 2.f));
        float r = max(other_r, my_r);
        if (dist_squared < r * r)
            return true;
        return false;
    }
}


void physicsUpdate(float elapsed_ms, vec2 window_size_in_game_units)
{
	// Move entities based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.


    for(auto &&[entity, motion, position]:  m_registry.view<Motion, Position>().each()) {
        float step_seconds = 1.0f * (elapsed_ms / 1000.f);
        position.position+=motion.velocity*step_seconds;
    }

	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	(void)window_size_in_game_units;


	if (DebugSystem::in_debug_mode)
	{
        for(auto &&[entity, motion, position]:  m_registry.view<Motion, Position>().each()) {
            auto scale_horizontal_line = position.scale;
            scale_horizontal_line.y *= 0.1f;
            auto scale_vertical_line = position.scale;
            scale_vertical_line.x *= 0.1f;
            DebugSystem::createLine(position.position, scale_horizontal_line);
            DebugSystem::createLine(position.position, scale_vertical_line);
        }
	}

    // Check for collisions between all moving entities
    auto view = m_registry.view<Position>();
	unsigned int size = view.size();
     for (unsigned int i = 0; i < size; i++){
         auto entity_i = view[i];
         auto &motion_i = view.get<Position>(entity_i);
         for (unsigned int j=i+1; j<size; j++)
         {
             auto entity_j = view[j];
             auto &motion_j = view.get<Position>(entity_j);
             if (collides(motion_i, motion_j))
             {}
         }

     }
}
