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
    
    auto texture = tifo::load_image(filename.c_str());
    if (texture == 0)
        return false;

    GLint tex_location;
    glGenTextures(1, &texture_id);TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D,texture_id);TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->sx, texture->sy, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->pixels);TEST_OPENGL_ERROR();
    tex_location = glGetUniformLocation(program_id, tex_variable.c_str());TEST_OPENGL_ERROR();
    glUniform1i(tex_location, uniform_index);TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();

    delete texture;
    return (texture_id != 0);
}
std::vector<Vertex> ParticleEffect::build_vertices()
{
    std::vector<Vertex> vertices;
    glm::vec3 X(0.5, 0, 0);
    glm::vec3 Y(0, 0.5, 0);
    glm::vec3 Z(0, 0, 1);

    for (auto &particle : particles)
    {
        glm::quat rotation = glm::angleAxis(particle.rotate, Z);

        glm::vec3 position0 = particle.position + (rotation * (-X - Y) * particle.size);//  * cameraRotation;
        glm::vec3 position1 = particle.position + (rotation * (X - Y) * particle.size);//  * cameraRotation;
        glm::vec3 position2 = particle.position + (rotation * (X + Y) * particle.size);//  * cameraRotation;
        glm::vec3 position3 = particle.position + (rotation * (-X + Y) * particle.size);//  * cameraRotation;

        // Bottom left
        Vertex v0(position0, particle.color, glm::vec2(0, 1));
        vertices.push_back(v0);
        // Bottom right
        Vertex v1(position1, particle.color, glm::vec2(1 ,1));
        vertices.push_back(v1);
        // Top right
        Vertex v2(position2, particle.color, glm::vec2(1, 0));
        vertices.push_back(v2);
        // Top left
        Vertex v3(position3, particle.color, glm::vec2(0, 0));
        vertices.push_back(v3);
    }

    return vertices;
}
std::vector<GLfloat> ParticleEffect::gl_build_vertices()
{
    std::vector<GLfloat> vertices;
    glm::vec3 X(0.5, 0, 0);
    glm::vec3 Y(0, 0.5, 0);
    glm::vec3 Z(0, 0, 1);

    size_t i = 0;
    for (auto &particle : particles)
    {
        glm::quat rotation = glm::angleAxis(particle.rotate, Z);

        glm::vec3 position0 = particle.position + (/*rotation **/ (-X - Y) * particle.size);//  * cameraRotation;
        glm::vec3 position1 = particle.position + (/*rotation **/ (X - Y) * particle.size);//  * cameraRotation;
        glm::vec3 position2 = particle.position + (/*rotation **/ (X + Y) * particle.size);//  * cameraRotation;
        glm::vec3 position3 = particle.position + (/*rotation **/ (-X + Y) * particle.size);//  * cameraRotation;

        if (i++ == 0)
        {
            std::cout << glm::to_string(rotation) << std::endl;
            std::cout << glm::to_string(particle.position) << std::endl;
            std::cout << particle.size << std::endl;

            std::cout << glm::to_string(position0) << std::endl;
            std::cout << glm::to_string(position1) << std::endl;
            std::cout << glm::to_string(position2) << std::endl;
            std::cout << glm::to_string(position3) << std::endl;
        }

        GLfloat *pos0 = glm::value_ptr(position0);
        vertices.push_back(pos0[0]);
        vertices.push_back(pos0[1]);
        vertices.push_back(pos0[2]);
        vertices.push_back(particle.color[0]);
        vertices.push_back(particle.color[1]);
        vertices.push_back(particle.color[2]);
        vertices.push_back(particle.color[3]);
        vertices.push_back(0);
        vertices.push_back(1);
        GLfloat *pos1 = glm::value_ptr(position1);
        vertices.push_back(pos1[0]);
        vertices.push_back(pos1[1]);
        vertices.push_back(pos1[2]);
        vertices.push_back(particle.color[0]);
        vertices.push_back(particle.color[1]);
        vertices.push_back(particle.color[2]);
        vertices.push_back(particle.color[3]);
        vertices.push_back(1);
        vertices.push_back(1);
        GLfloat *pos2 = glm::value_ptr(position2);
        vertices.push_back(pos2[0]);
        vertices.push_back(pos2[1]);
        vertices.push_back(pos2[2]);
        vertices.push_back(particle.color[0]);
        vertices.push_back(particle.color[1]);
        vertices.push_back(particle.color[2]);
        vertices.push_back(particle.color[3]);
        vertices.push_back(1);
        vertices.push_back(0);
        GLfloat *pos3 = glm::value_ptr(position3);
        vertices.push_back(pos3[0]);
        vertices.push_back(pos3[1]);
        vertices.push_back(pos3[2]);
        vertices.push_back(particle.color[0]);
        vertices.push_back(particle.color[1]);
        vertices.push_back(particle.color[2]);
        vertices.push_back(particle.color[3]);
        vertices.push_back(0);
        vertices.push_back(0);
    }
    return vertices;
}

void ParticleEffect::render_geometry()
{ 
    std::vector<GLfloat> vertices;
    for (const auto &particle : particles)
    {
        // std::cout << "particle.pos = " << particle.position[0]
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
    GLint particle_size_location = glGetUniformLocation(program_id, "size");
    TEST_OPENGL_ERROR();
    glUniform1f(particle_size_location, particles[0].size);
    TEST_OPENGL_ERROR();

    unsigned int VBO;
    glGenBuffers(1, &VBO);TEST_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);TEST_OPENGL_ERROR();

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat),
                          (void *)0);TEST_OPENGL_ERROR();
    glEnableVertexAttribArray(0);TEST_OPENGL_ERROR();

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);TEST_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, 0);TEST_OPENGL_ERROR();
    glDrawArrays(GL_POINTS, 0, vertices.size());TEST_OPENGL_ERROR();
}


void ParticleEffect::render(std::vector<GLfloat> &vertices)
{
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);TEST_OPENGL_ERROR();
    glGenBuffers(1, &EBO);TEST_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);TEST_OPENGL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);TEST_OPENGL_ERROR();

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)0);TEST_OPENGL_ERROR();
    glEnableVertexAttribArray(0);TEST_OPENGL_ERROR();

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);TEST_OPENGL_ERROR();

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);TEST_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, 0);TEST_OPENGL_ERROR();
    glDrawArrays( GL_TRIANGLES, 0, vertices.size());TEST_OPENGL_ERROR();
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
    std::cout << "nb_particles" << particles.size() << std::endl;
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
        particle.color += 0.5; // to fix - experiment
        particle.rotate = glm::lerp<float>(0.0f, 720.0f, life_ratio); // to make the particle face the camera
        particle.size = glm::lerp<float>(5.0f, 0.0f, life_ratio);
    }
}
