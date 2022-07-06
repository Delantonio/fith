#pragma once

#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

struct Particle
{
public:
    Particle()
        : position(0)
        , velocity(0)
        , force(0)
        , color(0.5)
        , size(1)
        , age(0)
        , lifeTime(0)
    {}

    Particle(glm::vec3 position, glm::vec3 velocity, glm::vec3 force,
             glm::vec4 color, float size, float age, float lifeTime)
        : position(position)
        , velocity(velocity)
        , force(force)
        , color(color)
        , size(size)
        , age(age)
        , lifeTime(lifeTime)
    {}

public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 force;
    glm::vec4 color;

    float size;
    float age;
    float lifeTime;
};
