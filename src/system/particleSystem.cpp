#include "particleSystem.hpp"
#include <iostream>

ParticleSystem::ParticleSystem(float speed)
    : killSpeed(speed)
{
}

void ParticleSystem::update()
{
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
    for (auto &entity: m_registry.view<Particle>()) {
        auto &p = m_registry.get<Particle>(entity);
        p.life -= killSpeed; // reduce life
        if (p.life > 0.0f)
        {    // particle is alive, thus update
            auto &pos = m_registry.get<Position>(entity);
            auto &motion = m_registry.get<Motion>(entity);
            pos.position -= motion.velocity;
            p.color.a -= killSpeed * 30.f;
        } else {
            m_registry.destroy(entity);
        }
    }
}


void ParticleSystem::emitParticle(vec2 pos, int amount)
{
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
    for (int i = 0; i < amount; i++){
        auto entity = m_registry.create();
        ShadedMesh &resource = cache_resource("particle");
        if (resource.effect.program.resource == 0)
            RenderSystem::createSprite(resource, textures_path("particle.png"), "particle");
        
        float random1 = ((rand() % 100) - 50) / 10.0f;
        float random2 = ((rand() % 100) - 50) / 10.0f;
        float rColor = 0.5f + ((rand() % 100) / 100.0f);
        
        m_registry.emplace<ShadedMeshRef>(entity, resource);
        auto &motion = m_registry.emplace<Motion>(entity);
        motion.velocity = {random1, random2};
        auto &position = m_registry.emplace<Position>(entity);
        position.position = pos + random1;
        position.angle = 0.f;
        position.scale = {35.f, 35.f};
        
        auto &particle = m_registry.emplace<Particle>(entity);
        particle.color = vec4(rColor, rColor, rColor, 1.0f);
        particle.life = 1.f;
    }
}
