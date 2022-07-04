#include "particle_effect.hh"
#include <cmath>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <random>
#include <utility>
#include "image_io.hh"
#include "particle.hh"

ParticleEffect::ParticleEffect(unsigned int nb_particles)
    : local_to_world(1),
    texture_id(0),
    force({0, 2, 0}) // gravity
{
    g_position = glm::vec3(0, 0, 5);
    this->n_particles = nb_particles;
    remaining_particles = nb_particles;
    update(0.033333f);
}
ParticleEffect::~ParticleEffect()
{
    glDeleteTextures(1, &texture_id);
    texture_id = 0;
}

void ParticleEffect::randomize()
{
    for (size_t i = 0; i < n_particles; i++)
        randomize(particles[i]);
}

void ParticleEffect::emit()
{
    for (auto &particle : particles)
    {
        sphere_emit(particle);
    }
}

bool ParticleEffect::load_texture(const std::string &filename, const std::string &tex_variable, GLuint &new_texture_id, GLint uniform_index)
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


    GLint tex_location;
    glGenTextures(1, &new_texture_id);TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D,new_texture_id);TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);TEST_OPENGL_ERROR();
    tex_location = glGetUniformLocation(program_id, tex_variable.c_str());TEST_OPENGL_ERROR();
    glUniform1i(tex_location, uniform_index);TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();

    texture_id = new_texture_id;

    // delete texture;
    stbi_image_free(texture);
    return (texture_id != 0);
}

void ParticleEffect::render_geometry()
{ 
    // if (particles.empty())
    //     return;
    std::vector<GLfloat> vertices;
    // int i = 0;
    for (const auto &particle : particles)
    {
        // std::cout << i++ << " particle.pos = " << particle.position[0]
        //     << " " << particle.position[1]
        //     << " " << particle.position[2] << std::endl;
        vertices.push_back(particle.position[0]);
        vertices.push_back(particle.position[1]);
        vertices.push_back(particle.position[2]);
        vertices.push_back(particle.color[0]);
        vertices.push_back(particle.color[1]);
        vertices.push_back(particle.color[2]);
        vertices.push_back(particle.color[3]);
    }
    // std::cout << "pushed back" << std::endl;

    GLint particle_size_location = glGetUniformLocation(program_id, "size");
    TEST_OPENGL_ERROR();
    glUniform1f(particle_size_location, particles[0].size);
    // glUniform1f(particle_size_location, 5);
    TEST_OPENGL_ERROR();

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);TEST_OPENGL_ERROR();
    unsigned int VBO;
    glGenBuffers(1, &VBO);TEST_OPENGL_ERROR();

    glBindVertexArray(VAO);TEST_OPENGL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);TEST_OPENGL_ERROR();

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();

    glEnableVertexAttribArray(0);TEST_OPENGL_ERROR();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat),
                          (void *)0);TEST_OPENGL_ERROR();

    glEnableVertexAttribArray(1);TEST_OPENGL_ERROR();
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)(3 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);TEST_OPENGL_ERROR();
    glDrawArrays(GL_POINTS, 0, vertices.size() / 7);TEST_OPENGL_ERROR();
    glDeleteBuffers(1, &VBO);
    glBindVertexArray(0);TEST_OPENGL_ERROR();
    glDisable(GL_BLEND);
}

void ParticleEffect::resize(unsigned int n_particles)
{
    particles.resize(n_particles);
}


// float random_range(int lower, int higher)
// {
//     return lower + (std::rand() % (higher - lower + 1));
// }

float random_range(float lower, float higher)
{
    return lower + static_cast<float>(rand()) * static_cast<float>(higher - lower) / static_cast<float>(RAND_MAX);
}

glm::vec3 random_vec()
{
    return glm::vec3(random_range(-1, 1), random_range(-1, 1), random_range(-1, 1));
}

void ParticleEffect::randomize(Particle &particle)
{
    particle.age = 0.0f;
    particle.lifeTime = random_range(3, 5);
    particle.position = random_vec(); // spawn point 
    particle.velocity = particle.position * random_range(10, 20);
    particle.size = 5.0f;
}
void ParticleEffect::emit(Particle &particle)
{
    std::cerr << "not implemented" << std::endl;
}
void ParticleEffect::sphere_emit(Particle &particle)
{
    float altitude = random_range(0.0f, M_PI);
    float azimuth = random_range(0.0f, 2 * M_PI);

    float radius = random_range(1, 3);
    float speed = random_range(1, 3);
    float lifeTime = random_range(5, 8);

    float sin_alt = sinf(altitude);

    float x = sin_alt * cosf(azimuth);
    float y = sin_alt * sinf(azimuth);
    float z = cosf(altitude);

    glm::vec3 vector(x, y, z);

    particle.position = (vector * radius) * g_position; 
    particle.velocity = vector * speed;
    particle.lifeTime = lifeTime;
    particle.age = 0;
    particle.size = 5.0f;
    particle.color.r = 1.0, particle.color.g = 0.8, particle.color.b = 0.5;
}
void ParticleEffect::disc_emit(Particle &particle, float radius)
{
    float distance = random_range(0.0, radius);
    float angle = random_range(0.0f, 2 * M_PI);

    float speed = random_range(1, 3);
    float lifeTime = random_range(mean_lifetime - lifetime_deviation, mean_lifetime + lifetime_deviation);

    glm::vec3 vector(distance * cos(angle) , random_range(0.0f, 0.5f), distance * sin(angle));

    particle.position = vector + g_position; 
    particle.velocity = glm::vec3(0, vector.y, 0) * speed;
    particle.lifeTime = lifeTime;
    particle.age = 0;
    particle.size = 5.0f;
    particle.color.r = 1.0, particle.color.g = 0.8, particle.color.b = 0.5;
}

float fclamp(float value, float lower, float higher)
{
    if (value < lower)
        return lower;
    if (value > higher)
        return higher;
    return value;
}



using limits = std::pair<float, float>;
using zone = std::pair<limits, glm::vec3>;

limits random_limits()
{
    float l1 = random_range(0, 1);
    float l2 = random_range(0, 1);
    return std::make_pair(std::min(l1, l2), std::max(l1, l2)); 
}

int respawned_particles = 0;
int nb_update = 0;
int max_wind_zones = 5;
std::mt19937 e2(7); // gaussian random generator
std::normal_distribution<> dist(0, 0.2);

std::vector<zone> wind_zones;
void ParticleEffect::update(float fDeltaTime)
{
    // std::cout << "nb_particles" << particles.size() << std::endl;
    nb_update++;
    int i = 0;
    float wind_chances = 0.1;
    bool add_wind = random_range(0, 1) < wind_chances; 
    if (add_wind && wind_zones.size() < max_wind_zones)
    {
        glm::vec3 wind_dir = glm::vec3(dist(e2), dist(e2) / 5, dist(e2)) * 0.1f; // maybe only x and z
        limits wind_lim = random_limits(); 
        std::cout << "new wind ! low = " << wind_lim.first
            << " high = " << wind_lim.second << std::endl;
        wind_zones.push_back(std::make_pair(wind_lim, wind_dir));
    }
    bool remove_wind = random_range(0, 1) < wind_chances; 
    if (remove_wind && wind_zones.size() != 0)
    {
        std::cout << "removed wind !" << std::endl;
        int index = random_range(0, wind_zones.size());
        wind_zones.erase(wind_zones.begin() + index);
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

        for (auto &wind_zone : wind_zones)
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
