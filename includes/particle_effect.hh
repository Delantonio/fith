#pragma once

#include "particle.hh"
#include "program.hh"

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

struct Vertex
{
    Vertex()
        : position(0), diffuse(0), uv(0)
    {}
    Vertex(glm::vec3 position, glm::vec3 diffuse, glm::vec2 uv)
        : position(position), diffuse(diffuse), uv(uv)
    {}

    glm::vec3 position;
    glm::vec3 diffuse;
    glm::vec2 uv;
};

class ParticleEffect
{
    public:
        ParticleEffect(unsigned int nb_particles = 0);
        virtual ~ParticleEffect();

        virtual void update(float fDeltaTime);
        //virtual void render();
        // virtual void render(std::vector<Vertex> &vertices);
        virtual void render(std::vector<GLfloat> &vertices);
        virtual void render_geometry();

        void randomize();
        void emit();

        bool load_texture(const std::string &filename, const std::string &tex_variable, GLuint &texture_id, GLint uniform_index);
        std::vector<Vertex> build_vertices();
        std::vector<GLfloat> gl_build_vertices();

        void resize(unsigned int n_particles);

    protected:
        void randomize(Particle &particle);
        void emit(Particle &particle);

        void sphere_emit(Particle &particle);

    public:
        unsigned int n_particles;
        std::vector<Particle> particles;
        glm::mat4 local_to_world;
        GLuint texture_id;
        glm::vec3 force;
        glm::vec3 g_position; // global position
};
