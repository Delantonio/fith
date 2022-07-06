#pragma once

#include <cmath>
#include <ctime>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <random>
#include <string>
#include <vector>

#include "particle.hh"
#include "program.hh"
#include "random.hh"

class ParticleEffect
{
public:
    ParticleEffect(glm::vec3 position, float radius,
                   unsigned int nb_particles = 0);
    virtual ~ParticleEffect();

    void update();
    void render();

    bool load_texture(GLuint &program_id, const std::string &filename,
                      const std::string &tex_variable, GLint uniform_index);
    void resize(unsigned int n_particles);

protected:
    virtual glm::vec3 force();
    virtual void effect_update(Particle &particle) {};

    virtual void emit(Particle &particle) {};
    void sphere_emit(Particle &particle);
    void disc_emit(Particle &particle);

public:
    glm::vec3 g_position;
    unsigned int n_particles;
    std::vector<Particle> particles;
    float radius;
    int remaining_particles;

    glm::vec3 mean_force = glm::vec3(0, -9.81, 0);
    glm::vec3 force_deviation = glm::vec3(0, 0, 0);
    int nb_update = 0;
    float mean_lifetime = 6.5;
    float lifetime_deviation = 1.0;
    float particle_size = 5.0;
    static float fDeltaTime;

private:
    GLuint texture_id;
};
