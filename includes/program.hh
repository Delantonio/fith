#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_std.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <memory>

#define TEST_OPENGL_ERROR()                                                             \
do {									\
    GLenum err = glGetError();					                        \
    if (err == GL_INVALID_OPERATION) std::cerr << "GL invalid operation " << __LINE__ << " " << __FILE__ << std::endl; \
    if (err != GL_NO_ERROR) std::cerr << err << " " << "OpenGL ERROR! " << __LINE__ << " " << gluErrorString(err) << " " << __FILE__ << std::endl; \
  } while(0)

extern GLuint fire_program_id;
extern GLuint object_program_id; 

std::string load(const std::string &filename);

class program
{
    public:
        program();
        ~program();
        program(program &&prog);
        static std::unique_ptr<program> make_program(std::string &vertex_shader, std::string &fragment_shader, std::string &geometry_shader);
        static std::unique_ptr<program> make_program(std::string &vertex_src, std::string &fragment_src);
        bool get_log(GLuint shader_id[]);
        bool is_ready();
        void use();

    public:
        GLuint program_id;
        GLint compile_status;
        GLint link_status;
};
