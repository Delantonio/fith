#include "program.hh"

std::string load(const std::string &filename) {
  std::ifstream input_src_file(filename, std::ios::in);
  std::string ligne;
  std::string file_content="";
  if (input_src_file.fail()) {
    std::cerr << "FAIL\n";
    return "";
  }
  while(getline(input_src_file, ligne)) {
    file_content = file_content + ligne + "\n";
  }
  file_content += '\0';
  input_src_file.close();
  return file_content;
}

program::program()
{
    program_id=glCreateProgram();TEST_OPENGL_ERROR();
}
program::~program()
{
    std::cout << "program delete - id = " << program_id << std::endl;
    // glDeleteProgram(program_id);TEST_OPENGL_ERROR();
    // program_id = 0;
}
std::unique_ptr<program> program::make_program(std::string &vertex_src,
                             std::string &fragment_src)
{
    auto prog = std::make_unique<program>();
    
    prog->compile_status = GL_TRUE;
    prog->link_status = GL_TRUE;
    GLuint shader_id[2];
    char *vertex_shd_src = (char*)std::malloc(vertex_src.length()*sizeof(char));
    char *fragment_shd_src = (char*)std::malloc(fragment_src.length()*sizeof(char));
    vertex_src.copy(vertex_shd_src,vertex_src.length());
    fragment_src.copy(fragment_shd_src,fragment_src.length());

    shader_id[0] = glCreateShader(GL_VERTEX_SHADER);TEST_OPENGL_ERROR();
    shader_id[1] = glCreateShader(GL_FRAGMENT_SHADER);TEST_OPENGL_ERROR();

    glShaderSource(shader_id[0], 1, (const GLchar**)&(vertex_shd_src), 0);TEST_OPENGL_ERROR();
    glShaderSource(shader_id[1], 1, (const GLchar**)&(fragment_shd_src), 0);TEST_OPENGL_ERROR();

    std::cout << "compilation..." << std::endl;
    for(int i = 0 ; i < 2 ; i++)
    {
        glCompileShader(shader_id[i]);TEST_OPENGL_ERROR();
    }
    std::cout << "done" << std::endl;
    
    std::cout << "logging..." << std::endl;
    if (!prog->get_log(shader_id, 2))
        return nullptr;
    std::cout << "done" << std::endl;
    std::free(vertex_shd_src);
    std::free(fragment_shd_src);

    prog->program_id=glCreateProgram();TEST_OPENGL_ERROR();
    std::cout << "prog->program_id = " << prog->program_id << std::endl;
    if (prog->program_id==0) 
        return nullptr;

    std::cout << "attaching..." << std::endl;
    for(int i = 0 ; i < 2 ; i++) {
        glAttachShader(prog->program_id, shader_id[i]);TEST_OPENGL_ERROR();
        std::cout << i << " shader_id" << shader_id[i] << std::endl;
    }
    std::cout << "done" << std::endl;
    std::cout << "linking..." << std::endl;
    glLinkProgram(prog->program_id);TEST_OPENGL_ERROR();
    std::cout << "done" << std::endl;

    glGetProgramiv(prog->program_id, GL_LINK_STATUS, &prog->link_status);TEST_OPENGL_ERROR();

    glDeleteShader(shader_id[0]);TEST_OPENGL_ERROR();
    glDeleteShader(shader_id[1]);TEST_OPENGL_ERROR();
    if (prog->link_status != GL_TRUE && !prog->get_log(shader_id, 2))
        return nullptr;

    return prog;
}

std::unique_ptr<program> program::make_program(std::string &vertex_src,
                             std::string &fragment_src, std::string &geometry_src)
{
    /*
        compiler les deux shaders
        invoquer l'éditeur de liens

        detruire les shaders à la fin
    */
    auto prog = std::make_unique<program>();
    
    prog->compile_status = GL_TRUE;
    prog->link_status = GL_TRUE;
    GLuint shader_id[3];
    char *vertex_shd_src = (char*)std::malloc(vertex_src.length()*sizeof(char));
    char *fragment_shd_src = (char*)std::malloc(fragment_src.length()*sizeof(char));
    char *geometry_shd_src = (char*)std::malloc(geometry_src.length()*sizeof(char));
    vertex_src.copy(vertex_shd_src,vertex_src.length());
    fragment_src.copy(fragment_shd_src,fragment_src.length());
    geometry_src.copy(geometry_shd_src, geometry_src.length());

    shader_id[0] = glCreateShader(GL_VERTEX_SHADER);TEST_OPENGL_ERROR();
    shader_id[2] = glCreateShader(GL_GEOMETRY_SHADER);TEST_OPENGL_ERROR();
    shader_id[1] = glCreateShader(GL_FRAGMENT_SHADER);TEST_OPENGL_ERROR();

    glShaderSource(shader_id[0], 1, (const GLchar**)&(vertex_shd_src), 0);TEST_OPENGL_ERROR();
    glShaderSource(shader_id[1], 1, (const GLchar**)&(fragment_shd_src), 0);TEST_OPENGL_ERROR();
    glShaderSource(shader_id[2], 1, (const GLchar**)&(geometry_shd_src), 0);TEST_OPENGL_ERROR();

    std::cout << "compilation..." << std::endl;
    for(int i = 0 ; i < 3 ; i++)
    {
        glCompileShader(shader_id[i]);TEST_OPENGL_ERROR();
    }
    std::cout << "done" << std::endl;
    
    std::cout << "logging..." << std::endl;
    if (!prog->get_log(shader_id, 3))
        return nullptr;
    std::cout << "done" << std::endl;
    std::free(vertex_shd_src);
    std::free(fragment_shd_src);
    std::free(geometry_shd_src);

    prog->program_id=glCreateProgram();TEST_OPENGL_ERROR();
    std::cout << "prog->program_id = " << prog->program_id << std::endl;
    if (prog->program_id==0) 
        return nullptr;

    std::cout << "attaching..." << std::endl;
    for(int i = 0 ; i < 3 ; i++) {
        glAttachShader(prog->program_id, shader_id[i]);TEST_OPENGL_ERROR();
    }
    std::cout << "done" << std::endl;
    std::cout << "linking..." << std::endl;
    glLinkProgram(prog->program_id);TEST_OPENGL_ERROR();
    std::cout << "done" << std::endl;

    glGetProgramiv(prog->program_id, GL_LINK_STATUS, &prog->link_status);TEST_OPENGL_ERROR();

    glDeleteShader(shader_id[0]);TEST_OPENGL_ERROR();
    glDeleteShader(shader_id[1]);TEST_OPENGL_ERROR();
    glDeleteShader(shader_id[2]);TEST_OPENGL_ERROR();
    if (prog->link_status != GL_TRUE && !prog->get_log(shader_id, 3))
        return nullptr;

    return prog;
}
bool program::get_log(GLuint shader_id[], int nb)
{
    /*
        renvoie log compilation ou edition de liens
    */
    for(int i = 0 ; i < nb ; i++) {
        glGetShaderiv(shader_id[i], GL_COMPILE_STATUS, &compile_status);
        if(compile_status != GL_TRUE) {
            GLint log_size;
            char *shader_log;
            glGetShaderiv(shader_id[i], GL_INFO_LOG_LENGTH, &log_size);
            shader_log = (char*)std::malloc(log_size+1); /* +1 pour le caractere de fin de chaine '\0' */
            if(shader_log != 0) {
                glGetShaderInfoLog(shader_id[i], log_size, &log_size, shader_log);
                std::cerr << "SHADER " << i << ": " << shader_log << std::endl;
                std::free(shader_log);
            }
            glDeleteShader(shader_id[i]);
            return false;
        }
    }
    if (link_status != GL_TRUE) {
        GLint log_size;
        char *program_log;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_size);
        program_log = (char*)std::malloc(log_size+1); /* +1 pour le caractere de fin de chaine '\0' */
        if(program_log != 0) {
            glGetProgramInfoLog(program_id, log_size, &log_size, program_log);
            std::cerr << "Program " << program_log << std::endl;
            std::free(program_log);
        }
        glDeleteProgram(program_id);TEST_OPENGL_ERROR();
        program_id=0;
        return false;
    }
    return true;
}
bool  program::is_ready()
{
    return compile_status && link_status;
}
void  program::use()
{
    glUseProgram(program_id);TEST_OPENGL_ERROR();
}
