#include "spark_effect.hh"

std::vector<zone> spark_wind_zones;

SparkEffect::SparkEffect(glm::vec3 position,
                       unsigned int nb_particles,
                       float radius,
                       unsigned int max_wind_zones)
    : ParticleEffect(position, nb_particles)
    , radius(radius)
    , max_wind_zones(max_wind_zones)
{
    force = glm::vec3(0, 2, 0); // Spark particles elevation
    e2 = std::mt19937(7); // gaussian random generator
    dist = std::normal_distribution<>(0, 0.2);
}

void SparkEffect::update(float fDeltaTime)
{
    // std::cout << "nb_particles" << particles.size() << std::endl;
    nb_update++;
    int i = 0;
    float wind_chances = 0.1;
    bool add_wind = random_range(0, 1) < wind_chances; 
    if (add_wind && spark_wind_zones.size() < max_wind_zones)
    {
        glm::vec3 wind_dir = glm::vec3(dist(e2), dist(e2) / 5, dist(e2)) * 0.1f; // maybe only x and z
        limits wind_lim = random_limits(); 
        std::cout << "new wind ! low = " << wind_lim.first
            << " high = " << wind_lim.second << std::endl;
        spark_wind_zones.push_back(std::make_pair(wind_lim, wind_dir));
    }
    bool remove_wind = random_range(0, 1) < wind_chances; 
    if (remove_wind && spark_wind_zones.size() != 0)
    {
        std::cout << "removed wind !" << std::endl;
        int index = random_range(0, spark_wind_zones.size());
        spark_wind_zones.erase(spark_wind_zones.begin() + index);
    }

    for (auto &particle : particles)
    {
        particle.age += fDeltaTime;
        if (particle.age >= particle.lifeTime)
        {
            respawned_particles++;
            disc_emit(particle, radius);
            continue;
        }

        // shape of the flame
        glm::vec3 center_dir = g_position - particle.position;
        float distance_center = std::sqrt(std::pow(center_dir.x, 2) + std::pow(center_dir.z, 2));
        particle.velocity += (force * fDeltaTime) ;
        particle.lifeTime -= 0.001 * distance_center;

        if ((nb_update + i) % 50 == 0)
        {
            float pertub_func_x = sinf(random_range(0.25f * M_PI, 0.75f * M_PI));
            pertub_func_x *= (rand() % 2) * 2 - 1;
            float pertub_func_z = sinf(random_range(0.25f * M_PI, 0.75f * M_PI));
            pertub_func_z *= (rand() % 2) * 2 - 1;
            glm::vec3 perturb_vec = glm::vec3(pertub_func_x, 0, pertub_func_z);
            particle.velocity += (0.1f * perturb_vec);
        }

        for (auto &wind_zone : spark_wind_zones)
        {
            // particle in wind_zone limits
            limits wind_limits = wind_zone.first;
            float life_ratio = particle.age / particle.lifeTime;
            if (life_ratio > wind_limits.first && life_ratio < wind_limits.second)
            {
                particle.velocity += wind_zone.second;
            }
        }

        particle.position += (particle.velocity * fDeltaTime);
        particle.color.x = 1 * (particle.lifeTime - (particle.age / 2)) / particle.lifeTime; // to fix - experiment
        particle.color.y = 0.8 * (particle.lifeTime - particle.age) / particle.lifeTime; // to fix - experiment
        particle.color.z = 0.5 * (particle.lifeTime - particle.age) / particle.lifeTime; // to fix - experiment
        // particle.size = glm::lerp<float>(5.0f, 5.0f, life_ratio);
        particle.size = 5; 
        i++;
    }
    // std::cout << "remaining_particles = " << remaining_particles
    //           << "respawned_particles = " << respawned_particles
    //           << " .size()= " << particles.size()
    //           << std::endl;
    if (remaining_particles > 0)
    {
        float needed_updates = mean_lifetime / fDeltaTime; 
        int spawning_particles = round(n_particles / needed_updates);
        int spawned_particles = n_particles - remaining_particles;
        resize(spawned_particles + spawning_particles);
        for (size_t i = 0; i < spawning_particles; i++)
        {
            disc_emit(particles[spawned_particles + i], radius);
        }
        remaining_particles -= spawning_particles;
    }
}
