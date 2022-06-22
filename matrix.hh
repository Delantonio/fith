#pragma once

#include <fstream>
#include <iostream>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

class matrix4
{
    public:
        matrix4();
        matrix4(const float &identity);
        void operator*=(const matrix4& rhs);
        static matrix4 identity();
        matrix4 frustum(const float &left, const float &right, const float &bottom, const float &top, const float &z_near, const float &z_far);
        matrix4 look_at(const float &eyeX, const float &eyeY, const float &eyeZ, const float &centerX, const float &centerY, const float &centerZ, const float &upX, const float &upY, const float &upZ);
    
    public:
        glm::mat4 matrix;
};
std::ostream& operator <<(std::ostream &out , const matrix4 &m);
void test_frustrum();
void test_look_at();
