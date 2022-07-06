#pragma once

#include <memory>

#include "flame_effect.hh"
#include "particle_effect.hh"
#include "smoke_effect.hh"
#include "spark_effect.hh"

struct zone
{
    limits wind_limits;
    glm::vec3 wind_force;
    float age;
};

class FireEffect : public ParticleEffect
{
public:
    FireEffect(glm::vec3 position, unsigned int nb_particles, float height,
               float radius, unsigned int max_wind_zones);

    void update();
    void render();
    bool load_texture(GLuint &program_id, const std::string &filename,
                      const std::string &tex_variable, GLint uniform_index);

private:
    void update_wind_zones();
    void apply_wind_forces(std::vector<Particle> &particles, float wind_power);

public:
    float height;
    unsigned int max_wind_zones;

    std::unique_ptr<FlameEffect> flame_effect;
    std::unique_ptr<SmokeEffect> smoke_effect;
    std::unique_ptr<SparkEffect> spark_effect;
    std::vector<zone> wind_zones;

private:
    float wind_chance = 0.1;
    std::mt19937 e2;
    std::normal_distribution<> dist; // gaussian random generator
};
