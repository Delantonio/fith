#pragma once

#include "particle_effect.hh"

class ParticleEmitter
{
    public:
        std::vector<ParticleEffect> effects; // smoke & fire
        glm::vec3 position;
};
