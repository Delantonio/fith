#include "random.hh"

float random_range(float lower, float higher)
{
    return lower + static_cast<float>(rand()) * static_cast<float>(higher - lower) / static_cast<float>(RAND_MAX);
}

limits random_limits(float min_height, float max_height)
{
    float l1 = random_range(min_height, max_height);
    float l2 = random_range(min_height, max_height);
    return std::make_pair(std::min(l1, l2), std::max(l1, l2)); 
}

glm::vec3 random_vec()
{
    return glm::vec3(random_range(-1, 1), random_range(-1, 1), random_range(-1, 1));
}

float fclamp(float value, float lower, float higher)
{
    if (value < lower)
        return lower;
    if (value > higher)
        return higher;
    return value;
}
