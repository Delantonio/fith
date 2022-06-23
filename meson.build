project('Fire in the Howl', 'cpp', default_options : ['cpp_std=c++20'])

# arguments = meson.get_compiler('cpp').get_supported_arguments('-w') # Hide warnings
# add_project_arguments(arguments,  language : 'cpp')

includes = [include_directories('includes')]
opengl = dependency('gl')
glew = dependency('glew')
glut = dependency('glut')

subdir('src')
executable('render', sources: sources, dependencies: [opengl, glew, glut] , include_directories: includes)
