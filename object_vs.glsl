#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec3 normals;
layout(location=2) in vec2 uv;

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;

uniform vec3 light_pos;
uniform vec3 light_color;

uniform vec3 color;

out vec3 out_color;
out vec2 uv_coord;

void main() {
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1.0f); 
    vec3 normal = normalize(normals) ;
    vec3 light_dir = normalize(light_pos - position);
    out_color = clamp( dot(normal, light_dir) * color, 0, 1);
    uv_coord = uv;
}
