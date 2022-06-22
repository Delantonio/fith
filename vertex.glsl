#version 450

/* vec3 light_color = vec3(1.0, 1.0, 1.0); */
/* vec3 light_position = vec3(1.0, 1.0, 5.0); */

/* mat4 model_view_matrix = mat4( */
/* 			      0.57735, -0.33333, 0.57735, 0.00000, */
/* 			      0.00000, 0.66667, 0.57735, 0.00000, */
/* 			      -0.57735, -0.33333, 0.57735, 0.00000, */
/* 			      0.00000, 0.00000, -17, 1.00000); */
/* mat4 projection_matrix = mat4( */
/* 			      5.00000, 0.00000, 0.00000, 0.00000, */
/* 			      0.00000, 5.00000, 0.00000, 0.00000, */
/* 			      0.00000, 0.00000, -1.00020, -1.00000, */
/* 			      0.00000, 0.00000, -10.00100, 0.00000); */

/* layout(location=0) in vec3 position; */
/* layout(location=1) in vec3 normals; */
/* layout(location=2) in vec2 uv; */

layout(location=0) in vec3 position;
layout(location=1) in vec4 p_color;
layout(location=2) in vec2 uv;

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;

uniform vec3 light_pos;
uniform vec3 light_color;

uniform vec3 color;

/* out vec3 out_color; */
out vec4 out_color;
out vec2 uv_coord;

void main() {
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1.0f); 
    /* vec3 normal = normalize(normals) ; */
    /* vec3 light_dir = normalize(light_pos - position); */
    /* out_color = clamp( dot(normal, light_dir) * color, 0, 1); */
    out_color = p_color;
    uv_coord = uv;
}
