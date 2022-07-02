#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "particle_effect.hh"
#include "program.hh"
#include "obj_loader.hh"
#include "image.hh"
#include "image_io.hh"

#include <GL/freeglut_std.h>

int vertices_size;
ParticleEffect particle_effect(1000);
float elapsed_time = 0.0f;
float delta_time = 0.033333f;
extern GLuint program_id = 0;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);TEST_OPENGL_ERROR();
    // std::cout << "drawing..." << std::endl;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // FILL or LINE to debug vertices and indices (backface culling)
    // glDrawElements(GL_TRIANGLES, vertices_size, GL_UNSIGNED_INT, 0);TEST_OPENGL_ERROR();

    // std::vector<GLfloat> vertices = particle_effect.gl_build_vertices();
    // particle_effect.render(vertices);
    particle_effect.render_geometry();

    glutSwapBuffers();TEST_OPENGL_ERROR();
    glutPostRedisplay();TEST_OPENGL_ERROR();
}
void idle()
{
    elapsed_time += delta_time;
    particle_effect.update(delta_time);
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
    glutInitWindowSize(1024, 1024);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("TP2 POGL");
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
    glClearColor(0.3,0.2,0.5,1.0);TEST_OPENGL_ERROR();
    return 1;
}

std::unique_ptr<program> init_shaders()
{
    std::string vertex_src = load("vertex.glsl");
    std::string fragment_src = load("fragment.glsl");
    std::string geometry_src = load("geometry.glsl");

    auto prog = program::make_program(vertex_src, fragment_src, geometry_src);
    if (prog->is_ready())
    {
        prog->use();
        std::cout << "shader program in use" << std::endl;
        return prog;
    }
    return nullptr;
}

bool init_object_vbo(std::shared_ptr<program> &prog)
{
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    std::string file = "trian_cylinder.obj";
    if (!load_obj(file, vertices, indices))
    {
        std::cerr << "Error while loading file" << std::endl;
        return -1;
    }
    vertices_size = indices.size();
    // for (unsigned int i = 0; i < vertices_size; i+=8)
    // {
    //     std::cout << "v " << vertices[i] << " " << vertices[i + 1] << " " << vertices[i + 2] << " ";
    //     std::cout << "vn " << vertices[i + 3] << " " << vertices[i + 4] << " " << vertices[i + 5] << " ";
    //     std::cout << "vt " << vertices[i + 6] << " " << vertices[i + 7];
    //     std::cout << std::endl;
    // }

    particle_effect.g_position = glm::vec3(0, 0, 5);
    GLuint texture1;
    particle_effect.load_texture("textures/red_glow_tr.tga", "particle_texture0", texture1, 0);

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);TEST_OPENGL_ERROR();
    glGenBuffers(1, &VBO);TEST_OPENGL_ERROR();
    glGenBuffers(1, &EBO);TEST_OPENGL_ERROR();

    glBindVertexArray(VAO);TEST_OPENGL_ERROR();
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

    glBindBuffer(GL_ARRAY_BUFFER, 0);TEST_OPENGL_ERROR();
    glBindVertexArray(0);TEST_OPENGL_ERROR();
    glBindVertexArray(VAO);TEST_OPENGL_ERROR();
    return true;
}

bool init_POV(std::shared_ptr<program> &prog)
{
    matrix4 id = matrix4::identity();
    GLint view_location = glGetUniformLocation(prog->program_id, "model_view_matrix");TEST_OPENGL_ERROR();
    std::cout << "view_location " << view_location << std::endl;
    matrix4 cam = id.look_at(3, 0, -5, 0, 0, 1, 0, 1, 0);
    glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(cam.matrix));
    TEST_OPENGL_ERROR();

    GLint projection = glGetUniformLocation(prog->program_id, "projection_matrix");TEST_OPENGL_ERROR();
    std::cout << "projection " << projection << std::endl;
    matrix4 proj = id.frustum(-1.0, 1.0, -1.0, 1.0, 1, 100);
    glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(proj.matrix));
    TEST_OPENGL_ERROR();

    // Set uniforms
    GLint color = glGetUniformLocation(prog->program_id, "color");
    const GLfloat frag_color[] = { 0.9, 0.16, 0.67 }; // pink
    std::cout << "color " << color << std::endl;
    glUniform3fv(color, 1, frag_color);
    TEST_OPENGL_ERROR();

    GLint light_pos = glGetUniformLocation(prog->program_id, "light_pos");
    const GLfloat light_position[] = { 1.0, 4.0, -3.0 }; // top right - no depth
    std::cout << "light_pos " << light_pos << std::endl;
    glUniform3fv(light_pos, 1, light_position);
    TEST_OPENGL_ERROR();

    GLint light_color = glGetUniformLocation(prog->program_id, "light_color");
    const GLfloat light_rgb[] = { 1.0, 1.0, 1.0 }; // white light
    std::cout << "light_color " << light_color << std::endl;
    glUniform3fv(light_color, 1, light_rgb);
    TEST_OPENGL_ERROR();

    return true;
}

bool init_textures()
{
    // tifo::rgb24_image *texture = tifo::load_image("textures/reddish.tga");
    tifo::rgb24_image *texture = tifo::load_image("textures/sparks.tga");
    GLuint texture_id;
    GLint tex_location;
    std::cout << "texture " << texture->sx << " ," <<  texture->sy << "\n";

    glGenTextures(1, &texture_id);TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D,texture_id);TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->sx, texture->sy, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->pixels);TEST_OPENGL_ERROR();
    tex_location = glGetUniformLocation(program_id, "texture0");TEST_OPENGL_ERROR();
    std::cout << "tex_location " << tex_location << std::endl;
    glUniform1i(tex_location,1);TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();

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
    std::shared_ptr<program> prog;
    if (!(prog = init_shaders()))
    {
        std::cerr << "Failed init_shaders" << std::endl;
        std::exit(-1);
    }
    program_id = prog->program_id;
    if (!init_object_vbo(prog))
    {
        std::cerr << "Failed init_object_vbo" << std::endl;
        std::exit(-1);
    }
    // test_frustrum();
    // test_look_at();
    init_POV(prog);
    // init_textures();
    glutMainLoop();
}
