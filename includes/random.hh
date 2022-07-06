#pragma once

#include <glm/ext/vector_float3.hpp>
#include <cstdlib>
#include <utility>
#include <algorithm>

using limits = std::pair<float, float>;

// Random utils
float random_range(float lower, float higher);
limits random_limits(float min_height, float max_height);
glm::vec3 random_vec();

// Utils
float fclamp(float value, float lower, float higher);
