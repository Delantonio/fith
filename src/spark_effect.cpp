#include "spark_effect.hh"
#include <cmath>
#include <glm/ext/vector_float3.hpp>
#include "random.hh"

SparkEffect::SparkEffect(glm::vec3 position, unsigned int nb_particles,
                         float height, float radius)
    : ParticleEffect(position, radius, nb_particles)
    , height(height)
{
    mean_force = glm::vec3(0, 0.4, 0);
    force_deviation = glm::vec3(0, 0.3, 0);
    particle_size = 3.0f * (radius / 10.0f);
    lifetime_deviation = 3.0f * (height / 60.0f);

    int nb_updates = 0;
    float incr = (mean_force.y + force_deviation.y) * std::pow(fDeltaTime, 2);
    for (float sim = 0.0f; sim < height; sim += nb_updates * incr)
        nb_updates++;
    float max_lifetime = nb_updates * fDeltaTime;
    mean_lifetime = max_lifetime - lifetime_deviation;
}

void SparkEffect::emit(Particle &particle)
{ 
    disc_emit(particle);

    particle.color.x = 1.0f;
    particle.color.y = 0.9f;
    particle.color.z = 0.2f;
}

void SparkEffect::effect_update(Particle &particle)
{
    glm::vec3 center_dir = g_position - particle.position;
    float distance_center = std::sqrt(std::pow(center_dir.x, 2) + std::pow(center_dir.z, 2));
    particle.age += 0.0003 * distance_center * (10.0f / radius);

    int particle_id = std::round(particle.lifeTime * 10);
    if ((nb_update + particle_id) % 50 == 0)
    {
        float pertub_func_x = sinf(random_range(0.25f * M_PI, 0.75f * M_PI));
        pertub_func_x *= (rand() % 2) * 2 - 1;
        float pertub_func_z = sinf(random_range(0.25f * M_PI, 0.75f * M_PI));
        pertub_func_z *= (rand() % 2) * 2 - 1;
        glm::vec3 perturb_vec = glm::vec3(pertub_func_x, 0, pertub_func_z);
        particle.velocity += (7.5f * perturb_vec * (radius / 10.0f));
    }
}


void SparkEffect::update()
{
    ParticleEffect::update();
    if (remaining_particles > 0)
    {
        float needed_updates = mean_lifetime / fDeltaTime;
        float nb_particles = n_particles / needed_updates;
        size_t spawning_particles = std::floor(nb_particles); 
        
        float extra_particle_proba = nb_particles - spawning_particles;
        spawning_particles += random_range(0, 1) < extra_particle_proba;
        size_t spawned_particles = n_particles - remaining_particles;

        resize(spawned_particles + spawning_particles);
        for (size_t i = 0; i < spawning_particles; i++)
            disc_emit(particles[spawned_particles + i]);
        remaining_particles -= spawning_particles;
    }
}
