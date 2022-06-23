#pragma once

#include <vector>
#include <map>
#include <array>
#include <GL/gl.h>
#include "matrix.hh"

bool load_obj(const std::string &path, std::vector<GLfloat> &output, std::vector<GLuint> &indices);
