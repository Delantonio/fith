#include "particle_effect.hh"
#include <cmath>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include "image_io.hh"
#include "particle.hh"

ParticleEffect::ParticleEffect(unsigned int nb_particles)
    : local_to_world(1),
    texture_id(0),
    force({0, -9.81, 0}) // gravity
{
    g_position = glm::vec3(0, 0, 3);
    resize(nb_particles);
    update(0.033333f);
}
ParticleEffect::~ParticleEffect()
{
    if (texture_id != 0)
    {
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }
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

bool ParticleEffect::load_texture(const std::string &filename, const std::string &tex_variable, GLuint &texture_id, GLint uniform_index)
{
    if (texture_id != 0)
    {
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }
    
    // auto texture = tifo::load_image(filename.c_str());
    int width, height, bpp;
    auto texture = stbi_load(filename.c_str(), &width, &height, &bpp, 4);
    if (texture == 0)
        return false;

    GLint tex_location;
    glGenTextures(1, &texture_id);TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D,texture_id);TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);TEST_OPENGL_ERROR();
    tex_location = glGetUniformLocation(program_id, tex_variable.c_str());TEST_OPENGL_ERROR();
    glUniform1i(tex_location, uniform_index);TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();

    // delete texture;
    stbi_image_free(texture);
    return (texture_id != 0);
}

void ParticleEffect::render_geometry()
{ 
    // if (particles.empty())
    //     return;
    std::cout << "render geometry" << std::endl;
    std::vector<GLfloat> vertices;
    int i = 0;
    for (const auto &particle : particles)
    {
        std::cout << i++ << " particle.pos = " << particle.position[0]
            << " " << particle.position[1]
            << " " << particle.position[2] << std::endl;
        vertices.push_back(particle.position[0]);
        vertices.push_back(particle.position[1]);
        vertices.push_back(particle.position[2]);
        // vertices.push_back(particle.color[0]);
        // vertices.push_back(particle.color[1]);
        // vertices.push_back(particle.color[2]);
        // vertices.push_back(particle.color[3]);
    }
    std::cout << "pushed back" << std::endl;
    vertices.erase(vertices.begin());
    vertices.erase(vertices.begin());
    vertices.erase(vertices.begin());
    GLint particle_size_location = glGetUniformLocation(program_id, "size");
    TEST_OPENGL_ERROR();
    // glUniform1f(particle_size_location, particles[0].size);
    glUniform1f(particle_size_location, 5);
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
    for (size_t i = 0; i < vertices.size(); i++)
    {
        std::cout << "vertice " << i << " = " << vertices[i] << std::endl; 
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (void *)0);TEST_OPENGL_ERROR();
    glEnableVertexAttribArray(0);TEST_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, 0);TEST_OPENGL_ERROR();
    glDrawArrays(GL_POINTS, 0, vertices.size());TEST_OPENGL_ERROR();
    glDeleteBuffers(1, &VBO);
    glBindVertexArray(0);TEST_OPENGL_ERROR();
}

void ParticleEffect::resize(unsigned int n_particles)
{
    this->n_particles = n_particles;
    particles.resize(n_particles);
}


float random_range(int lower, int higher)
{
    return lower + (std::rand() % (higher - lower + 1));
}

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
    float altitude = glm::radians(random_range(0.0f, M_PI));
    float azimuth = glm::radians(random_range(0.0f, 2 * M_PI));

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
    particle.color.r = 0.5, particle.color.g = 0.5, particle.color.b = 0.5;
}

float fclamp(float value, float lower, float higher)
{
    if (value < lower)
        return lower;
    if (value > higher)
        return higher;
    return value;
}

void ParticleEffect::update(float fDeltaTime)
{
    // std::cout << "nb_particles" << particles.size() << std::endl;
    for (auto &particle : particles)
    {
        particle.age += fDeltaTime;
        if (particle.age >= particle.lifeTime)
        {
            // or emit with emitter
            // randomize(particle);
            sphere_emit(particle);
        }
        float life_ratio = fclamp(particle.age / particle.lifeTime, 0, 1); 
        particle.velocity += (force * fDeltaTime) ;
        particle.position += (particle.velocity * fDeltaTime);
        // std::cout << "pos = " 
        //     << particle.position[0] << " "
        //     << particle.position[1] << " "
        //     << particle.position[2] << std::endl;
        particle.color += 0.5; // to fix - experiment
        particle.rotate = glm::lerp<float>(0.0f, 720.0f, life_ratio); // to make the particle face the camera
        // particle.size = glm::lerp<float>(5.0f, 0.0f, life_ratio);
    }
}
