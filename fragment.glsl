#version 450

uniform sampler2D texture_sampler;
/* uniform sampler2D lighting_sampler; */
/* uniform sampler2D normalmap_sampler; */

/* in vec3 out_color; */
in vec4 out_color;
layout(location=0) out vec4 output_color;
in vec2 uv_coord;

void main() {
    /* output_color = vec4(out_color, 1.0) + texture(texture_sampler, uv_coord);  */
    output_color = out_color + texture(texture_sampler, uv_coord);
}
