#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec3 normals;
layout(location=2) in vec2 uv;

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;

uniform vec3 light_pos;
uniform vec3 light_color;

out VS_OUT
{
    vec3 position;
    vec3 normals;
    vec2 uv;
    vec3 light_pos;
} vs_out;

void main() {
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1.0f); 
    /* vec3 normal = normalize(normals) ; */
    /* vec3 light_dir = light_pos - position; */
    /* float dist = length(light_dir); */
    /* out_color = clamp( dot(normal, light_dir) * color, 0, 1); */
    /* float light_coeff = 1.0 / (1.0 + 0.01 * pow(dist, 2)); */
    /* out_color = clamp( dot(normal, normalize(light_dir)) * light_color * light_coeff, 0.1, 1); */

    vs_out.position = position;
    vs_out.normals = normals;
    vs_out.uv = uv;
    vs_out.light_pos = light_pos;
}
