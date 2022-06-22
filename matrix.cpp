#include "matrix.hh"


matrix4::matrix4()
{
    matrix = glm::mat4(0.0f);
}
matrix4::matrix4(const float &identity)
{
    matrix = glm::mat4(identity);
}
void matrix4::operator *=(const matrix4& rhs)
{
    matrix = matrix * rhs.matrix;
}

matrix4 matrix4::identity()
{
    return matrix4(1.0f);
}
std::ostream& operator <<(std::ostream &out , const matrix4 &m)
{
    return out << glm::to_string(m.matrix);
}
matrix4 matrix4::frustum(const float &left, const float &right, const float &bottom, const float &top, const float &z_near, const float &z_far)
{
    matrix4 mat;
    float a = (right + left) / (right - left);
    float b = (top + bottom) / (top - bottom);
    float c = -1 * (z_far + z_near) / (z_far - z_near);
    float d = -1 * ((2 * z_far * z_near) / (z_far - z_near));

    mat.matrix[0][0] = (2 * z_near) / (right - left);
    mat.matrix[1][1] = (2 * z_near) / (top - bottom);
    mat.matrix[2][0] = a;
    mat.matrix[2][1] = b;
    mat.matrix[2][2] = c;
    mat.matrix[3][2] = d;
    mat.matrix[2][3] = -1;

    return mat;
}
matrix4 matrix4::look_at(const float &eyeX, const float &eyeY, const float &eyeZ, const float &centerX, const float &centerY, const float &centerZ, const float &upX, const float &upY, const float &upZ)
{
    glm::vec3 eye(eyeX, eyeY, eyeZ);
    glm::vec3 center(centerX, centerY, centerZ);
    glm::vec3 up(upX, upY, upZ);

    glm::vec3 f = glm::normalize(center - eye);
    glm::vec3 s = glm::cross(f, glm::normalize(up));
    glm::vec3 u = glm::cross(glm::normalize(s), f);

    matrix4 mat;
    mat.matrix[0][0] = s[0];
    mat.matrix[0][1] = u[0];
    mat.matrix[0][2] = -f[0];
    mat.matrix[1][0] = s[1];
    mat.matrix[1][1] = u[1];
    mat.matrix[1][2] = -f[1];
    mat.matrix[2][0] = s[2];
    mat.matrix[2][1] = u[2];
    mat.matrix[2][2] = -f[2];
    mat.matrix[3][3] = 1;

    mat.matrix = glm::translate(mat.matrix, -eye);
    return mat;
}

void test_frustrum()
{
    matrix4 id = matrix4();
    matrix4 mat = id.frustum(-5, 5, -5, 5, 1, 100);
    std::cout << mat << std::endl;
    matrix4 mat2 = id.frustum(-3, 3, -6, 6, 2, 300);
    std::cout << mat2 << std::endl;
}

void test_look_at()
{
    matrix4 id = matrix4();
    matrix4 mat = id.look_at(5, 6, 7, 13, 14, 15, 0, 1, 0);
    std::cout << mat << std::endl;

    matrix4 mat2 = id.look_at(5, 6, 7, 13, 14, 15, 1, 0, 0);
    std::cout << mat2 << std::endl;
}
