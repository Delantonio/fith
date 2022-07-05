#pragma once

#include "particle_effect.hh"

class FireEffect : public ParticleEffect
{
    public:
        FireEffect(glm::vec3 position, unsigned int nb_particles, float radius, unsigned int max_wind_zones);
        void update(float fDeltaTime);

    public:
        float radius;
        unsigned int max_wind_zones;
    protected:
        int respawned_particles = 0;
        int nb_update = 0;
        std::mt19937 e2; // gaussian random generator
        std::normal_distribution<> dist;
};
