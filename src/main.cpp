#include <cstdlib>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "fire_effect.hh"
#include "particle_effect.hh"
#include "program.hh"
#include "obj_loader.hh"
#include "image.hh"
#include "image_io.hh"

#include <GL/freeglut_std.h>

int vertices_size = 0;
float elapsed_time = 0.0f;

float ParticleEffect::fDeltaTime = 0.033333f;

// FireEffect(position, nb_particles, height, radius, max_wind_zones)
FireEffect fire_effect({0, -3, 5}, 3000, 9.0f, 1.0f, 5);

GLuint object_program_id = 0;
GLuint fire_program_id = 0;

std::vector<GLfloat> vertices;
std::vector<GLuint> indices;
GLuint scene_texture_id;
void DrawScene();

unsigned int scene_VAO;
unsigned int particles_VAO = 0;

int display_count = 0;
void display()
{
    if (++display_count <= 100000)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);TEST_OPENGL_ERROR();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // FILL or LINE to debug vertices and indices (backface culling)

        // Draw scene
        glUseProgram(object_program_id);TEST_OPENGL_ERROR();
        DrawScene();

        // Draw particles
        glUseProgram(fire_program_id);
        fire_effect.render();

        glutSwapBuffers();TEST_OPENGL_ERROR();
        glutPostRedisplay();TEST_OPENGL_ERROR();
    }
}

void idle()
{
    if (elapsed_time < 0.03f)
        std::cout << "update !" << std::endl;
    elapsed_time += ParticleEffect::fDeltaTime;
    fire_effect.update();
}

void window_resize(int width, int height)
{
    glViewport(0, 0, width, height); // TEST_OPENGL_ERROR();
}

bool init_glut(int &argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitContextVersion(4, 5);
    glutInitContextProfile(GLUT_CORE_PROFILE | GLUT_DEBUG);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1080, 1080);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Fire In The Howl - OpenGL");
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(window_resize);
    return true;
}

bool init_glew()
{
    return (glewInit()==GLEW_OK);
}

bool init_gl()
{
    glEnable(GL_DEPTH_TEST);TEST_OPENGL_ERROR();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);TEST_OPENGL_ERROR();
    // glEnable(GL_CULL_FACE);TEST_OPENGL_ERROR();
    glClearColor(17.0 / 255.0 ,17.0 / 255.0 ,58.0 / 255.0 ,1.0);TEST_OPENGL_ERROR();
    return 1;
}

bool init_shaders()
{
    std::string object_vertex_src = load("object_vs.glsl");
    std::string object_fragment_src = load("object_fs.glsl");

    auto object_prog = program::make_program(object_vertex_src, object_fragment_src);
    object_program_id = object_prog->program_id;
    if (!object_prog->is_ready())
        return false;

    // object_prog->use();
    // std::cout << "object shader program in use" << std::endl;

    std::string fire_vertex_src = load("fire_vs.glsl");
    std::string fire_fragment_src = load("fire_fs.glsl");
    std::string fire_geometry_src = load("fire_gs.glsl");

    auto fire_prog = program::make_program(fire_vertex_src, fire_fragment_src, fire_geometry_src);
    fire_program_id = fire_prog->program_id;
    if (!fire_prog->is_ready())
        return false;

    // fire_prog->use();
    // std::cout << "fire shader program in use" << std::endl;

    GLint fire_shader_nb = 0;
    GLint obj_shader_nb = 0;
    glGetProgramiv(fire_program_id, GL_ATTACHED_SHADERS, &fire_shader_nb);TEST_OPENGL_ERROR();
    glGetProgramiv(object_program_id, GL_ATTACHED_SHADERS, &obj_shader_nb);TEST_OPENGL_ERROR();

    std::cout << "nb fire = " << fire_shader_nb << std::endl;
    std::cout << "nb obj = " << obj_shader_nb << std::endl;

    return true;
}

void add_floor_vertice(glm::vec3 position, glm::vec3 normal, glm::vec2 uv_coord)
{
    auto pos = glm::value_ptr(position);
    vertices.push_back(pos[0]);
    vertices.push_back(pos[1]);
    vertices.push_back(pos[2]);
    auto n = glm::value_ptr(normal);
    vertices.push_back(n[0]);
    vertices.push_back(n[1]);
    vertices.push_back(n[2]);
    auto uv = glm::value_ptr(uv_coord); 
    vertices.push_back(uv[0]);
    vertices.push_back(uv[1]);
}

void displace_vertices(glm::vec3 displacement)
{
    auto d = glm::value_ptr(displacement);
    for (size_t i = 0; i < vertices.size(); i+=8)
    {
        vertices[i] += d[0];
        vertices[i + 1] += d[1];
        vertices[i + 2] += d[2];
    }
}

void add_floor(float floor_size)
{
    glm::vec3 X(0.5, 0.0, 0.0);
    glm::vec3 Y(0.0, 1.0, 0.0);
    glm::vec3 Z(0.0, 0.0, 0.5);
    glm::vec3 position0 = fire_effect.g_position - Y + (-X - Z) * floor_size;  
    glm::vec3 position1 = fire_effect.g_position - Y + (X - Z) * floor_size;
    glm::vec3 position2 = fire_effect.g_position - Y + (-X + Z) * floor_size;
    glm::vec3 position3 = fire_effect.g_position - Y + (X + Z) * floor_size;

    add_floor_vertice(position0, Y, glm::vec2(0, 0)); // bas gauche
    add_floor_vertice(position1, Y, glm::vec2(1, 0)); // bas droite
    add_floor_vertice(position2, Y, glm::vec2(0, 1)); // haut gauche
    add_floor_vertice(position3, Y, glm::vec2(1, 1)); // haut droite

    GLuint max = 0;
    for (const auto &indice : indices)
        if (indice > max)
            max = indice;
    // 0, 1, 2 first triangle
    indices.push_back(max + 1);
    indices.push_back(max + 2);
    indices.push_back(max + 3);

    // 1, 2, 3 second triangle
    indices.push_back(max + 2);
    indices.push_back(max + 3);
    indices.push_back(max + 4);
}

bool init_object_vbo()
{
    std::string file = "landscape.obj";
    if (!load_obj(file, vertices, indices))
    {
        std::cerr << "Error while loading file" << std::endl;
        return -1;
    }
    displace_vertices(glm::vec3(0, fire_effect.g_position.y, fire_effect.g_position.z));

    // Floor must be added after the objects
    // add_floor(100.0);

    vertices_size = indices.size();

    fire_effect.load_texture(fire_program_id, "textures/white_glow_tr.tga", "particle_texture0", 0);

    glGenVertexArrays(1, &scene_VAO);TEST_OPENGL_ERROR();
    glGenVertexArrays(1, &particles_VAO);TEST_OPENGL_ERROR();

    return true;
}

bool init_POV()
{
    matrix4 id = matrix4::identity();
    matrix4 model_view = id.look_at(0, 0, -3, 0, 0, 1, 0, 1, 0);
    // matrix4 proj = id.frustum(-1.0, 1.0, -1.0, 1.0, 1, 1000);
    glm::mat4 mProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 1000.0f);

    glUseProgram(fire_program_id);TEST_OPENGL_ERROR();
    GLint fire_mv_loc = glGetUniformLocation(fire_program_id, "model_view_matrix");TEST_OPENGL_ERROR();
    std::cout << "view_location " << fire_mv_loc << std::endl;
    glUniformMatrix4fv(fire_mv_loc, 1, GL_FALSE, glm::value_ptr(model_view.matrix));TEST_OPENGL_ERROR();
    
    GLint fire_proj_loc = glGetUniformLocation(fire_program_id, "projection_matrix");TEST_OPENGL_ERROR();
    std::cout << "projection " << fire_proj_loc << std::endl;
    // glUniformMatrix4fv(fire_proj_loc, 1, GL_FALSE, glm::value_ptr(proj.matrix));TEST_OPENGL_ERROR();
    glUniformMatrix4fv(fire_proj_loc, 1, GL_FALSE, glm::value_ptr(mProjection));TEST_OPENGL_ERROR();

    glUseProgram(object_program_id);TEST_OPENGL_ERROR();
    std::cout << "object_program_id = " << object_program_id << std::endl;
    GLint object_mv_loc = glGetUniformLocation(object_program_id, "model_view_matrix");TEST_OPENGL_ERROR();
    std::cout << "view_location " << object_mv_loc << std::endl;
    glUniformMatrix4fv(object_mv_loc, 1, GL_FALSE, glm::value_ptr(model_view.matrix));TEST_OPENGL_ERROR();

    GLint object_proj_loc = glGetUniformLocation(object_program_id, "projection_matrix");TEST_OPENGL_ERROR();
    std::cout << "projection " << object_proj_loc << std::endl;
    glUniformMatrix4fv(object_proj_loc, 1, GL_FALSE, glm::value_ptr(mProjection));TEST_OPENGL_ERROR();

    GLint light_pos = glGetUniformLocation(object_program_id, "light_pos");TEST_OPENGL_ERROR();
    // const GLfloat light_position[] = { 1.0, 4.0, -3.0 }; // top right - no depth
    std::cout << "light_pos " << light_pos << std::endl;
    // glUniform3fv(light_pos, 1, light_position);TEST_OPENGL_ERROR();
    // glUniform3fv(light_pos, 1, glm::value_ptr(fire_effect.g_position + glm::vec3(0, fire_effect.height, 0)));TEST_OPENGL_ERROR();
    glUniform3fv(light_pos, 1, glm::value_ptr(fire_effect.g_position + glm::vec3(0, 0.8f, 0)));TEST_OPENGL_ERROR();

    GLint light_color = glGetUniformLocation(object_program_id, "light_color");TEST_OPENGL_ERROR();
    const GLfloat light_rgb[] = { 1.0, 0.8, 0.5 }; // white light
    std::cout << "light_color " << light_color << std::endl;
    glUniform3fv(light_color, 1, light_rgb);TEST_OPENGL_ERROR();

    return true;
}

bool init_textures()
{
    int width, height, bpp;
    // auto texture = stbi_load("textures/minecraft_brick.jpg", &width, &height, &bpp, 3);
    auto texture = stbi_load("textures/forest/2K-forest_albedo.jpg", &width, &height, &bpp, 3);
    if (texture == 0)
        return false;

    GLint tex_location;

    glGenTextures(1, &scene_texture_id);TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D,scene_texture_id);TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);TEST_OPENGL_ERROR();
    tex_location = glGetUniformLocation(object_program_id, "texture_sampler");TEST_OPENGL_ERROR();
    std::cout << "object tex_location " << tex_location << std::endl;
    glUniform1i(tex_location, 0);TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();

    glBindTexture(GL_TEXTURE_2D, 0);TEST_OPENGL_ERROR();

    return true;
}

int main(int argc, char *argv[])
{
    std::srand(7);
    if (!init_glut(argc, argv))
    {
        std::cerr << "Failed init_glew" << std::endl;
        std::exit(-1);
    }
    if (!init_glew())
    {
        std::cerr << "Failed init_glew" << std::endl;
        std::exit(-1);
    }
    if (!init_gl())
    {
        std::cerr << "Failed init_gl" << std::endl;
        std::exit(-1);
    }
    if (!init_shaders())
    {
        std::cerr << "Failed init_shaders" << std::endl;
        std::exit(-1);
    }

    GLint fire_shader_nb = 0;
    GLint obj_shader_nb = 0;
    glGetProgramiv(fire_program_id, GL_ATTACHED_SHADERS, &fire_shader_nb);
    glGetProgramiv(object_program_id, GL_ATTACHED_SHADERS, &obj_shader_nb);

    std::cout << "MAIN nb fire = " << fire_shader_nb << std::endl;
    std::cout << "MAIN nb obj = " << obj_shader_nb << std::endl;

    if (!init_object_vbo())
    {
        std::cerr << "Failed init_object_vbo" << std::endl;
        std::exit(-1);
    }
    // test_frustrum();
    // test_look_at();
    init_POV();
    init_textures();
    glutMainLoop();
}

void DrawScene()
{
    glBindTexture(GL_TEXTURE_2D, scene_texture_id);TEST_OPENGL_ERROR();

    glBindVertexArray(scene_VAO);TEST_OPENGL_ERROR();

    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);TEST_OPENGL_ERROR();
    glGenBuffers(1, &EBO);TEST_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);TEST_OPENGL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);TEST_OPENGL_ERROR();

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(),
                 GL_STATIC_DRAW);TEST_OPENGL_ERROR();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);TEST_OPENGL_ERROR();
    glEnableVertexAttribArray(0);TEST_OPENGL_ERROR();

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);TEST_OPENGL_ERROR();

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);TEST_OPENGL_ERROR();

    glBindVertexArray(scene_VAO);TEST_OPENGL_ERROR();
    glDrawElements(GL_TRIANGLES, vertices_size, GL_UNSIGNED_INT, 0);TEST_OPENGL_ERROR();

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glBindVertexArray(0);TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, 0);TEST_OPENGL_ERROR();
}
