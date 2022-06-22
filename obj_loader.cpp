#include "obj_loader.hh"

bool load_obj(const std::string &path, std::vector<GLfloat> &output, std::vector<GLuint> &indices)
{
    std::ifstream file(path);
    std::string line;
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> uvs;
    std::vector<GLfloat> normals;
    using VertIndices = std::array<size_t, 3>;
    std::vector<std::array<VertIndices, 3>> faces;

    while (file >> line)
    {
        GLfloat v1;
        GLfloat v2;
        GLfloat v3;
        if (!line.compare("v"))
        {
            file >> v1 >> v2 >> v3;
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
        }
        else if (!line.compare("vt"))
        {
            file >> v1 >> v2;
            uvs.push_back(v1);
            uvs.push_back(v2);
        }
        else if (!line.compare("vn"))
        {
            file >> v1 >> v2 >> v3;
            normals.push_back(v1);
            normals.push_back(v2);
            normals.push_back(v3);
        }
        else if (!line.compare("f"))
        {
            char slash; // unused
            size_t v1, vn1, vt1, v2, vn2, vt2, v3, vn3, vt3;
            file >> v1 >> slash >> vt1 >> slash >> vn1;
            VertIndices vertices_indices1 = {v1 - 1, vt1 - 1, vn1 - 1};
            file >> v2 >> slash >> vt2 >> slash >> vn2;
            VertIndices vertices_indices2 = {v2 - 1, vt2 - 1, vn2 - 1};
            file >> v3 >> slash >> vt3 >> slash >> vn3;
            VertIndices vertices_indices3 = {v3 - 1, vt3 - 1, vn3 - 1};
            std::array<VertIndices, 3> face = {vertices_indices1, vertices_indices2, vertices_indices3};
            faces.push_back(face);
        }
    }

    std::map<VertIndices, size_t> index_translator;

    size_t i = 0;
    for (const auto &face : faces)
    {
        for (const auto &vert_indices : face)
        {
            auto result = index_translator.insert({vert_indices, i});

            auto v = vert_indices[0];
            auto vt = vert_indices[1];
            auto vn = vert_indices[2];
            if (result.second)
            {
                output.push_back(vertices[v * 3]);
                output.push_back(vertices[v * 3 + 1]);
                output.push_back(vertices[v * 3 + 2]);
                output.push_back(normals[vn * 3]);
                output.push_back(normals[vn * 3 + 1]);
                output.push_back(normals[vn * 3 + 2]);
                output.push_back(uvs[vt * 2]);
                output.push_back(uvs[vt * 2 + 1]);
                i++;
            }
            indices.push_back(result.first->second);
        }
    }

    return true;
}
