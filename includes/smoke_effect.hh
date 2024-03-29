#pragma once

#include "particle_effect.hh"

class SmokeEffect : public ParticleEffect
{
public:
    SmokeEffect(glm::vec3 position, unsigned int nb_particles, float height,
                float radius);
    void update();

protected:
    glm::vec3 force() override;
    void effect_update(Particle &particle) override;
    void emit(Particle &particle) override;

public:
    float height;
    int respawned_particles = 0;
};
