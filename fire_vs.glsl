#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec4 p_color;

uniform mat4 model_view_matrix;

/* uniform vec3 light_pos; */
/* uniform vec3 light_color; */

out vec4 color;

void main() {
    gl_Position = model_view_matrix * vec4(position, 1.0f); 
    color = p_color;
}
