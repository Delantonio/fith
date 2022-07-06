#include "particle_effect.hh"

#include <cmath>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>
#include <math.h>
#include <random>
#include <utility>

#include "image_io.hh"
#include "particle.hh"
#include "random.hh"
#include "program.hh"

ParticleEffect::ParticleEffect(glm::vec3 position, float radius,
                               unsigned int nb_particles)
    : g_position(position)
    , n_particles(nb_particles)
    , radius(radius)
    , remaining_particles(nb_particles)
    , texture_id(0)

{
    update();
}

ParticleEffect::~ParticleEffect()
{
    glDeleteTextures(1, &texture_id);
    texture_id = 0;
}

bool ParticleEffect::load_texture(GLuint &program_id,
                                  const std::string &filename,
                                  const std::string &tex_variable,
                                  GLint uniform_index)
{
    if (texture_id != 0)
    {
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }

    int width, height, bpp;
    auto texture = stbi_load(filename.c_str(), &width, &height, &bpp, 4);
    if (texture == 0)
        return false;

    GLuint new_texture_id;

    GLint tex_location;
    glGenTextures(1, &new_texture_id);TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D,new_texture_id);TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);TEST_OPENGL_ERROR();

    glUseProgram(program_id);
    tex_location = glGetUniformLocation(program_id, tex_variable.c_str());TEST_OPENGL_ERROR();
    std::cout << "load_texture -> " << program_id << std::endl;
    glUniform1i(tex_location, uniform_index);TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    TEST_OPENGL_ERROR();

    glBindTexture(GL_TEXTURE_2D, 0);TEST_OPENGL_ERROR();

    texture_id = new_texture_id;

    // delete texture;
    stbi_image_free(texture);
    return (texture_id != 0);
}

void ParticleEffect::render()
{ 
    if (!particles.size())
        return;
    std::vector<GLfloat> vertices;
    for (const auto &particle : particles)
    {
        vertices.push_back(particle.position[0]);
        vertices.push_back(particle.position[1]);
        vertices.push_back(particle.position[2]);
        vertices.push_back(particle.color[0]);
        vertices.push_back(particle.color[1]);
        vertices.push_back(particle.color[2]);
        vertices.push_back(particle.color[3]);
    }

    GLint particle_size_location =
        glGetUniformLocation(fire_program_id, "size");
    TEST_OPENGL_ERROR();
    glUniform1f(particle_size_location, particles[0].size);
    TEST_OPENGL_ERROR();

    glBindTexture(GL_TEXTURE_2D, texture_id);
    TEST_OPENGL_ERROR();

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    TEST_OPENGL_ERROR();
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    TEST_OPENGL_ERROR();

    glBindVertexArray(VAO);
    TEST_OPENGL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    TEST_OPENGL_ERROR();

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                 vertices.data(), GL_STATIC_DRAW);
    TEST_OPENGL_ERROR();

    glEnableVertexAttribArray(0);
    TEST_OPENGL_ERROR();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat),
                          (void *)0);
    TEST_OPENGL_ERROR();

    glEnableVertexAttribArray(1);
    TEST_OPENGL_ERROR();
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)(3 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    TEST_OPENGL_ERROR();
    glDrawArrays(GL_POINTS, 0, vertices.size() / 7);
    TEST_OPENGL_ERROR();

    glDeleteBuffers(1, &VBO);
    glBindVertexArray(0);
    TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, 0);
    TEST_OPENGL_ERROR();
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void ParticleEffect::resize(unsigned int n_particles)
{
    particles.resize(n_particles);
}

void ParticleEffect::sphere_emit(Particle &particle)
{
    float altitude = random_range(0.0f, M_PI);
    float azimuth = random_range(0.0f, 2 * M_PI);

    float speed = random_range(1, 3);
    float lifeTime = random_range(5, 8);

    float sin_alt = sinf(altitude);

    float x = sin_alt * cosf(azimuth);
    float y = sin_alt * sinf(azimuth);
    float z = cosf(altitude);

    glm::vec3 vector(x, y, z);

    particle.position = (vector * radius) * g_position;
    particle.velocity = vector * speed;
    particle.force = force();
    particle.lifeTime = lifeTime;
    particle.age = 0;
    particle.size = particle_size;
    particle.color.r = 1.0, particle.color.g = 0.8, particle.color.b = 0.5;
}

void ParticleEffect::disc_emit(Particle &particle)
{
    float distance = random_range(0.0, radius);
    float angle = random_range(0.0f, 2 * M_PI);

    float speed = random_range(1, 3);
    float lifeTime = random_range(mean_lifetime - lifetime_deviation,
                                  mean_lifetime + lifetime_deviation);

    glm::vec3 vector(distance * cos(angle), random_range(0.0f, 0.5f),
                     distance * sin(angle));

    particle.position = vector + g_position;
    particle.velocity = glm::vec3(0, vector.y, 0) * speed;
    particle.force = force();
    particle.lifeTime = lifeTime;
    particle.age = 0;
    particle.size = particle_size;
    particle.color.r = 1.0, particle.color.g = 0.8, particle.color.b = 0.5;
}

glm::vec3 ParticleEffect::force()
{
    float x = random_range(mean_force.x - force_deviation.x,
                           mean_force.x + force_deviation.x);
    float y = random_range(mean_force.y - force_deviation.y,
                           mean_force.y + force_deviation.y);
    float z = random_range(mean_force.z - force_deviation.z,
                           mean_force.z + force_deviation.z);
    return glm::vec3(x, y , z);
}

void ParticleEffect::update()
{
    nb_update++;
    for (auto &particle : particles)
    {
        particle.age += fDeltaTime;
        if (particle.age >= particle.lifeTime)
            emit(particle);
        else
        {
            effect_update(particle);
            particle.velocity += (particle.force * fDeltaTime);
            particle.position += (particle.velocity * fDeltaTime);
        }
    }
}
