#include "fire_effect.hh"

#include <cmath>
#include <random>

float delta_30_fps = 0.033333f;
float max_wind_delay = 0.3f;

FireEffect::FireEffect(glm::vec3 position, unsigned int nb_particles,
                       float height, float radius, unsigned int max_wind_zones)
    : ParticleEffect(position, radius, nb_particles)
    , height(height)
    , max_wind_zones(max_wind_zones)
{
    dist = std::normal_distribution<>(0, 0.75);
    int flame_particles = std::round(7.5 * nb_particles / 10) - 10;
    int smoke_particles = std::round(2.5 * nb_particles / 10);
    int spark_particles = 15;
    flame_effect = std::make_unique<FlameEffect>(position, flame_particles,
                                                 3.75 * height / 8, radius);
    smoke_effect = std::make_unique<SmokeEffect>(position, smoke_particles,
                                                 6 * height / 8, radius * 0.9f);
    spark_effect = std::make_unique<SparkEffect>(position, spark_particles,
                                                 height, radius);
}

void FireEffect::render()
{
    flame_effect->render();
    smoke_effect->render();
    spark_effect->render();
}

bool FireEffect::load_texture(GLuint &program_id, const std::string &filename,
                              const std::string &tex_variable,
                              GLint uniform_index)
{
    bool flame_result = flame_effect->load_texture(program_id, filename,
                                                   tex_variable, uniform_index);
    bool smoke_result = smoke_effect->load_texture(program_id, filename,
                                                   tex_variable, uniform_index);
    bool spark_result = spark_effect->load_texture(program_id, filename,
                                                   tex_variable, uniform_index);
    return flame_result && smoke_result && spark_result;
}

void FireEffect::update_wind_zones()
{
    for (auto &wind_zone : wind_zones)
        wind_zone.age += fDeltaTime;

    float wind_chances = 0.02 * (fDeltaTime / delta_30_fps);
    bool add_wind = random_range(0, 1) < wind_chances;
    if (add_wind && wind_zones.size() < max_wind_zones)
    {
        glm::vec3 wind_dir = glm::vec3(dist(e2), dist(e2) / 5, dist(e2)) * 0.1f;
        limits wind_lim = random_limits(0, height);
        wind_zones.push_back(zone{ wind_lim, wind_dir, 0. });
    }

    bool remove_wind = random_range(0, 1) < wind_chances;
    if (remove_wind && wind_zones.size() != 0)
    {
        int index = random_range(0, wind_zones.size());
        if (wind_zones[index].age > max_wind_delay + fDeltaTime)
            wind_zones.erase(wind_zones.begin() + index);
    }
}

void FireEffect::apply_wind_forces(std::vector<Particle> &particles,
                                   float wind_delay)
{
    for (auto &particle : particles)
    {
        for (auto &wind_zone : wind_zones)
        {
            if (wind_zone.age < wind_delay)
                continue;

            float lower = g_position.y + wind_zone.wind_limits.first;
            float upper = g_position.y + wind_zone.wind_limits.second;
            if (particle.position.y > lower && particle.position.y < upper)
            {
                float wind_factor =
                    (fDeltaTime / delta_30_fps) * (radius / 10.0f) * 3;
                particle.velocity += wind_zone.wind_force * wind_factor;
            }
        }
    }
}

void FireEffect::update()
{
    nb_update++;
    update_wind_zones();

    apply_wind_forces(flame_effect->particles, max_wind_delay);
    apply_wind_forces(smoke_effect->particles, 0.0f);
    apply_wind_forces(spark_effect->particles, 0.0f);

    flame_effect->update();
    smoke_effect->update();
    spark_effect->update();
}
