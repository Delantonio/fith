#pragma once

#include "particle.hh"
#include "program.hh"
#include "random.hh"

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include <string>
#include <ctime>
#include <cmath>
#include <random>

using zone = std::pair<limits, glm::vec3>;

class ParticleEffect
{
    public:
        ParticleEffect(glm::vec3 position, unsigned int nb_particles = 0);
        virtual ~ParticleEffect();

        virtual void update(float fDeltaTime);
        void render();

        void randomize();
        void emit();

        bool load_texture(GLuint &program_id, const std::string &filename, const std::string &tex_variable, GLuint &texture_id, GLint uniform_index);

        void resize(unsigned int n_particles);

    protected:
        void randomize(Particle &particle);
        void emit(Particle &particle);

        void sphere_emit(Particle &particle);
        void disc_emit(Particle &particle, float radius);

    public:
        unsigned int n_particles;
        std::vector<Particle> particles;
        int remaining_particles;
        glm::vec3 force;
        glm::vec3 g_position; // global position
        float mean_lifetime = 6.5;
        float lifetime_deviation = 0;
        // float radius = 10.0;
    private:
        GLuint texture_id;
};
