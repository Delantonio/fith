#version 450

uniform sampler2D texture0;
uniform sampler2D particle_texture0;

layout(location=0) out vec4 frag_color;
in vec4 vertex_color;
in vec2 vertex_uv;

void main() {
    vec2 uv = vertex_uv;
    uv.y *= -1.0;
    /* frag_color = vertex_color + texture(texture0, uv); */
    frag_color = texture(particle_texture0, uv);
}
