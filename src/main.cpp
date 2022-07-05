#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "fire_effect.hh"
#include "smoke_effect.hh"
#include "program.hh"
#include "obj_loader.hh"
#include "image.hh"
#include "image_io.hh"

#include <GL/freeglut_std.h>

int vertices_size;
float elapsed_time = 0.0f;
float delta_time = 0.033333f;
extern GLuint fire_program_id = 0;
extern GLuint object_program_id = 0;

// FireEffect(position, nb_particles, radius, max_wind_zones)
FireEffect flame_particle_effect({0, -30, 50}, 2000, 10.0f, 5);

SmokeEffect smoke_particle_effect({0, -30, 50}, 200, 8.0f, 5);

int display_count = 0;

void display()
{
    if (++display_count <= 100000)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);TEST_OPENGL_ERROR();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // FILL or LINE to debug vertices and indices (backface culling)

        // Draw particle
        glUseProgram(fire_program_id);
        smoke_particle_effect.render();
        flame_particle_effect.render();

        // Draw scene
        // glUseProgram(object_program_id);
        // glDrawElements(GL_TRIANGLES, vertices_size, GL_UNSIGNED_INT, 0);TEST_OPENGL_ERROR();

        glutSwapBuffers();TEST_OPENGL_ERROR();
        glutPostRedisplay();TEST_OPENGL_ERROR();
    }
}
void idle()
{
    if (elapsed_time < 0.03f)
        std::cout << "update !" << std::endl;
    elapsed_time += delta_time;
    flame_particle_effect.update(delta_time);
    smoke_particle_effect.update(delta_time);
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
    glClearColor(0.3,0.2,0.5,1.0);TEST_OPENGL_ERROR();
    return 1;
}

bool init_shaders()
{
    std::string fire_vertex_src = load("fire_vs.glsl");
    std::string fire_fragment_src = load("fire_fs.glsl");
    std::string fire_geometry_src = load("fire_gs.glsl");

    auto fire_prog = program::make_program(fire_vertex_src, fire_fragment_src, fire_geometry_src);
    fire_program_id = fire_prog->program_id;
    if (!fire_prog->is_ready())
        return false;

    fire_prog->use();
    std::cout << "fire shader program in use" << std::endl;

    // std::string object_vertex_src = load("object_vs.glsl");
    // std::string object_fragment_src = load("object_fs.glsl");

    // auto object_prog = program::make_program(object_vertex_src, object_fragment_src);
    // object_program_id = object_prog->program_id;
    // if (!object_prog->is_ready())
    //     return false;

    // object_prog->use();
    // std::cout << "object shader program in use" << std::endl;

    return true;
}

bool init_object_vbo()
{
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    std::string file = "trian_cylinder.obj";
    // if (!load_obj(file, vertices, indices))
    // {
    //     std::cerr << "Error while loading file" << std::endl;
    //     return -1;
    // }
    // vertices_size = indices.size();

    GLuint texture1;
    GLuint texture2;
    flame_particle_effect.load_texture(fire_program_id, "textures/white_glow_tr.tga", "particle_texture0", texture1, 0);
    smoke_particle_effect.load_texture(fire_program_id, "textures/white_glow_tr.tga", "particle_texture0", texture2, 0);

    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);TEST_OPENGL_ERROR();

    // unsigned int VBO, VAO, EBO;
    // glGenVertexArrays(1, &VAO);TEST_OPENGL_ERROR();
    // glGenBuffers(1, &VBO);TEST_OPENGL_ERROR();
    // glGenBuffers(1, &EBO);TEST_OPENGL_ERROR();

    // glBindVertexArray(VAO);TEST_OPENGL_ERROR();
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);TEST_OPENGL_ERROR();
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);TEST_OPENGL_ERROR();

    // glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(),
    //              GL_STATIC_DRAW);TEST_OPENGL_ERROR();

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
    //                       (void *)0);TEST_OPENGL_ERROR();
    // glEnableVertexAttribArray(0);TEST_OPENGL_ERROR();

    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
    //                       (void *)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);TEST_OPENGL_ERROR();

    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
    //                       (void *)(6 * sizeof(float)));
    // glEnableVertexAttribArray(2);TEST_OPENGL_ERROR();

    // glBindVertexArray(0);TEST_OPENGL_ERROR();
    // glBindVertexArray(VAO);TEST_OPENGL_ERROR();
    return true;
}

bool init_POV()
{
    matrix4 id = matrix4::identity();
    matrix4 model_view = id.look_at(0, 0, -5, 0, 0, 1, 0, 1, 0);
    matrix4 proj = id.frustum(-1.0, 1.0, -1.0, 1.0, 1, 100);

    GLint fire_mv_loc = glGetUniformLocation(fire_program_id, "model_view_matrix");TEST_OPENGL_ERROR();
    std::cout << "view_location " << fire_mv_loc << std::endl;
    glUniformMatrix4fv(fire_mv_loc, 1, GL_FALSE, glm::value_ptr(model_view.matrix));
    TEST_OPENGL_ERROR();

    GLint fire_proj_loc = glGetUniformLocation(fire_program_id, "projection_matrix");TEST_OPENGL_ERROR();
    std::cout << "projection " << fire_proj_loc << std::endl;
    glUniformMatrix4fv(fire_proj_loc, 1, GL_FALSE, glm::value_ptr(proj.matrix));
    TEST_OPENGL_ERROR();

    // GLint object_mv_loc = glGetUniformLocation(object_program_id, "model_view_matrix");TEST_OPENGL_ERROR();
    // std::cout << "view_location " << object_mv_loc << std::endl;
    // glUniformMatrix4fv(object_mv_loc, 1, GL_FALSE, glm::value_ptr(model_view.matrix));
    // TEST_OPENGL_ERROR();

    // GLint object_proj_loc = glGetUniformLocation(object_program_id, "projection_matrix");TEST_OPENGL_ERROR();
    // std::cout << "projection " << object_proj_loc << std::endl;
    // glUniformMatrix4fv(object_proj_loc, 1, GL_FALSE, glm::value_ptr(proj.matrix));
    // TEST_OPENGL_ERROR();

    // // Set uniforms
    // GLint color = glGetUniformLocation(fire_program_id, "color");
    // const GLfloat frag_color[] = { 0.9, 0.16, 0.67 }; // pink
    // std::cout << "color " << color << std::endl;
    // glUniform3fv(color, 1, frag_color);
    // TEST_OPENGL_ERROR();

    // GLint light_pos = glGetUniformLocation(fire_program_id, "light_pos");
    // const GLfloat light_position[] = { 1.0, 4.0, -3.0 }; // top right - no depth
    // std::cout << "light_pos " << light_pos << std::endl;
    // glUniform3fv(light_pos, 1, light_position);
    // TEST_OPENGL_ERROR();

    // GLint light_color = glGetUniformLocation(fire_program_id, "light_color");
    // const GLfloat light_rgb[] = { 1.0, 1.0, 1.0 }; // white light
    // std::cout << "light_color " << light_color << std::endl;
    // glUniform3fv(light_color, 1, light_rgb);
    // TEST_OPENGL_ERROR();

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
    tex_location = glGetUniformLocation(object_program_id, "texture_sampler");TEST_OPENGL_ERROR();
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
    if (!init_shaders())
    {
        std::cerr << "Failed init_shaders" << std::endl;
        std::exit(-1);
    }
    if (!init_object_vbo())
    {
        std::cerr << "Failed init_object_vbo" << std::endl;
        std::exit(-1);
    }
    // test_frustrum();
    // test_look_at();
    init_POV();
    // init_textures();
    glutMainLoop();
}
