#pragma once

#include "particle.hh"
#include "particle_effect.hh"

class FlameEffect : public ParticleEffect
{
public:
    FlameEffect(glm::vec3 position, unsigned int nb_particles, float height,
                float radius);
    void update();

protected:
    void effect_update(Particle &particle) override;
    void emit(Particle &particle) override;

public:
    float height;
    int respawned_particles = 0;
};
