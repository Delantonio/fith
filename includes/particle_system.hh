#pragma once

#include "particle_effect.hh"

class ParticleSystem
{
    public:
        ParticleSystem();
        ~ParticleSystem();
        void render();
        void update(float fDeltaTime);

    public:
        glm::vec3 position;
        std::vector<ParticleEffect> effects; // smoke & fire & amber
};
