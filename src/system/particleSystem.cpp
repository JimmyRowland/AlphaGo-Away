#include "particleSystem.hpp"
#include <iostream>

ParticleSystem::ParticleSystem(float speed)
    : killSpeed(speed)
{
}

vec2 Clamp(vec2 ori, float fixedLength)
{
	float v_length = sqrt(ori.x * ori.x + ori.y * ori.y);

	float scale = v_length / fixedLength;

	return { (float)(ori.x / scale), (float)(ori.y / scale) };
}

float get_mass(Position position){
    return pow(position.scale.y,2);
}
void perfect_elastic_collision(entt::entity entity_i, entt::entity entity_j){
    auto&& [position_i, motion_i] = m_registry.get<Position, Motion>(entity_i);
    auto&& [position_j, motion_j] = m_registry.get<Position, Motion>(entity_j);
    float mass_i = get_mass(position_i);
    float mass_j = get_mass(position_j);
    float mass_sum = mass_i + mass_j;
    motion_i.velocity = motion_i.velocity -
                        (position_i.position-position_j.position)
                        * (2*mass_j)/mass_sum
                        *glm::dot(motion_i.velocity-motion_j.velocity, position_i.position-position_j.position)
                        /(float) pow(glm::distance(position_i.position,position_j.position),2);
    motion_j.velocity = motion_j.velocity +
                        (position_j.position-position_i.position)
                        * (2*mass_i)/mass_sum
                        *glm::dot(motion_j.velocity-motion_i.velocity, position_j.position-position_i.position)
                        /(float) pow(glm::distance(position_j.position,position_i.position),2);
    position_i.position = position_j.position + glm::normalize(position_i.position-position_j.position)*((position_i.scale.y + position_j.scale.y)/4+1);
}

bool bounces(const Position& position1, const Position& position2)
{
    return glm::distance(position1.position,position2.position) <= position1.scale.y/4 + position2.scale.y/4;
}

float gravitational_constant = 0.1;

vec2 getGravitationalAcceleration(Position position){
    vec2 acceleration = vec2(0,0);
    for(auto&&[entity, unit_property, unit_position]: m_registry.view<UnitProperty, Position>().each()){
        vec2 difference = unit_position.position - position.position;
        float distance = glm::length(difference);
        acceleration += glm::normalize(difference)* (float) (gravitational_constant * unit_property.hp/pow(distance,1));
    }
    return acceleration;
}


void ParticleSystem::update()
{
    for (auto &cur: m_registry.view<Particle>()) {

		vec2 acceleration = { 0, 0 };

        auto &p = m_registry.get<Particle>(cur);
        p.life -= killSpeed; // reduce life
//		p.life -= 0.1f; // reduce life

        if (p.life > 0.0f)
        {    // particle is alive, thus update
			auto &curpos = m_registry.get<Position>(cur);
			auto &curmot = m_registry.get<Motion>(cur);
			if(gravity_toggle){
                acceleration+=getGravitationalAcceleration(curpos);
            }

			if (p.life < 0.99f && swarm_behavior_toggle)
			{

				int count = 0;
				vec2 avgSpeed = { 0, 0 };
				vec2 avgPos = { 0, 0 };

				for (auto &entity : m_registry.view<Particle>())
				{
					auto &np = m_registry.get<Particle>(entity);
					auto &pos = m_registry.get<Position>(entity);
					auto &mot = m_registry.get<Motion>(entity);


					float distance = sqrt((pos.position.x - curpos.position.x)*(pos.position.x - curpos.position.x) + (pos.position.y - curpos.position.y)*(pos.position.y - curpos.position.y));

					if (np.life < 0.99f && entity != cur && distance < MAX_DISTANCE)
					{
						avgSpeed = { avgSpeed.x + mot.velocity.x, avgSpeed.y + mot.velocity.y };
						avgPos = { avgPos.x + pos.position.x, avgPos.y + pos.position.y };
						count++;
					}
				}
				if (count > 0)
				{

					avgSpeed = { avgSpeed.x / count, avgSpeed.y / count };

					avgSpeed = Clamp(avgSpeed, 1);

					acceleration += vec2( avgSpeed.x - curmot.velocity.x, avgSpeed.y - curmot.velocity.y );

					acceleration = Clamp(acceleration, FIXED_SPEED);
				}
			}

			curmot.velocity = { curmot.velocity.x + acceleration.x, curmot.velocity.y + acceleration.y };
            if(swarm_behavior_toggle){
                curmot.velocity = Clamp(curmot.velocity, 5);
            }

			curpos.position += curmot.velocity;
            if(is_out_of_boundary(cur)){
                m_registry.destroy(cur);
            }
            p.color.a -= killSpeed * 10.f;
        } else {
            m_registry.destroy(cur);
        }
    }

    if(elastic_collision_toggle){
        auto entities = m_registry.view<Particle>();
        for (unsigned int i=0; i<entities.size(); i++)
        {

            Position& motion_i = m_registry.get<Position>(entities[i]);
            for (unsigned int j=i+1; j<entities.size(); j++)
            {

                Position& motion_j = m_registry.get<Position>(entities[j]);
                if (bounces(motion_i, motion_j))
                {
                    perfect_elastic_collision(entities[i],entities[j]);
                }

            }

        }
    }






}


void ParticleSystem::emitParticle(vec2 pos, int amount)
{
    for (int i = 0; i < amount; i++){
        auto entity = m_registry.create();
        ShadedMesh &resource = cache_resource("particle");
        if (resource.effect.program.resource == 0)
            RenderSystem::createSprite(resource, textures_path("particle.png"), "particle");



        float random1 = ((rand() % 100) - 50) / 10.0f;
        float random2 = ((rand() % 100) - 50) / 10.0f;
        float rColor = 0.5f + ((rand() % 100) / 100.0f);
        float rScale = 35.f * (rand()%20)/10;
        
        m_registry.emplace<ShadedMeshRef>(entity, resource);
        auto &motion = m_registry.emplace<Motion>(entity);
        motion.velocity = {random1, random2};
        auto &position = m_registry.emplace<Position>(entity);
        position.position = pos + random1;
        position.angle = 0.f;
        position.scale = {rScale, rScale};
        
        auto &particle = m_registry.emplace<Particle>(entity);
        particle.color = vec4(rColor, rColor, rColor, 1.0f);
        particle.life = 1.f;
    }
}



// add new particles
//    for (unsigned int i = 0; i < newParticles; ++i)
//    {
//        int unusedParticle = this->firstUnusedParticle();
//        this->respawnParticle(this->particles[unusedParticle], pos,
//                              offset);
//    }
//    // update all particles
//    for (unsigned int i = 0; i < this->amount; ++i)
//    {
//        p.life -= dt; // reduce life
//        if (p.life > 0.0f)
//        {    // particle is alive, thus update
//            auto &pos = m_registry.get<Position>(this->particles[i]);
//            auto &motion = m_registry.get<Motion>(this->particles[i]);
//            pos.position -= motion.velocity;
//            p.color.a -= dt * 2.5f;
//        }
//    }




//    float random = ((rand() % 100) - 50) / 10.0f;
//    float rColor = 0.5f + ((rand() % 100) / 100.0f);
//    auto &position = m_registry.get<Position>(particle_e);
//    auto &motion = m_registry.get<Motion>(particle_e);
//    position.position = pos + random + offset;
//    motion.velocity = {random, random};
//
//    auto &particle = m_registry.get<Particle>(particle_e);
//    particle.color = vec4(rColor, rColor, rColor, 1.0f);
//    particle.life = 1.0f;