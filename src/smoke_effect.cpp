#include "smoke_effect.hh"
#include "particle_effect.hh"

SmokeEffect::SmokeEffect(glm::vec3 position, unsigned int nb_particles,
                         float height, float radius)
    : ParticleEffect(position, radius, nb_particles)
    , height(height)
{
    mean_force = glm::vec3(0, 0.5, 0);
    force_deviation = glm::vec3(0, 0.3, 0);
    particle_size = 10.0f * (radius / 10.0f);
    lifetime_deviation = 0.5f * (height / 60.0f);

    int nb_updates = 0;
    float incr = (mean_force.y + force_deviation.y) * std::pow(fDeltaTime, 2);
    for (float sim = 0.0f; sim < height; sim += nb_updates * incr)
        nb_updates++;
    float max_lifetime = nb_updates * fDeltaTime;
    mean_lifetime = max_lifetime - lifetime_deviation;
}

void SmokeEffect::emit(Particle &particle)
{
    disc_emit(particle);
}

float gauss_func(float x, float amplitude = 0.15f, float center = 0.8f,
                 float stdev = 0.3f)
{
    float exp_fact = std::pow(x - center, 2) / (2 * std::pow(stdev, 2));
    return amplitude * std::exp(-exp_fact);
}

glm::vec3 SmokeEffect::force()
{
    if (random_range(0, 1) < 0.1)
        return glm::vec3(0, random_range(1.6, 2.2), 0);
    return ParticleEffect::force();
}

void SmokeEffect::effect_update(Particle &particle)
{
    glm::vec3 center_dir = g_position - particle.position;
    float distance_center =
        std::sqrt(std::pow(center_dir.x, 2) + std::pow(center_dir.z, 2));
    particle.age += 0.0003 * distance_center * (10.0f / radius);

    int particle_id = std::round(particle.lifeTime * 10);
    if ((nb_update + particle_id) % 50 == 0)
    {
        float pertub_func_x = sinf(random_range(0.25f * M_PI, 0.75f * M_PI));
        pertub_func_x *= (rand() % 2) * 2 - 1;
        float pertub_func_z = sinf(random_range(0.25f * M_PI, 0.75f * M_PI));
        pertub_func_z *= (rand() % 2) * 2 - 1;
        glm::vec3 perturb_vec = glm::vec3(pertub_func_x, 0, pertub_func_z);
        particle.velocity += (1.0f * perturb_vec * (radius / 10.0f));
    }

    particle.color.x = 1 * (particle.lifeTime - (particle.age * 1.3));
    particle.color.y = 0.8 * (particle.lifeTime - (particle.age * 1.1));
    particle.color.z = 0.5 * (particle.lifeTime - particle.age);

    particle.color = particle.color / particle.lifeTime + 0.2f;
    particle.color.x = std::max(particle.color.x, particle.color.z);
    particle.color.y = std::max(particle.color.y, particle.color.z);

    float height_ratio = (particle.position.y - g_position.y) / height;
    if (height_ratio < 0.6)
    {
        particle.color.w =
            std::max(0.1f, fclamp(gauss_func(height_ratio), 0, 1));
        center_dir.y = 0;
        float spread = std::pow(1.0f - height_ratio, 20) * 1.5f;
        particle.position -= glm::normalize(center_dir) * fDeltaTime * spread;
    }
    else
        particle.color.w = fclamp(gauss_func(height_ratio), 0, 1);
}

void SmokeEffect::update()
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
