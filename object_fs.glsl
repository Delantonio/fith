#version 450

uniform sampler2D texture_sampler;
/* uniform sampler2D lighting_sampler; */
/* uniform sampler2D normalmap_sampler; */

in VS_OUT
{
    vec3 position;
    vec3 normals;
    vec2 uv;
    vec3 light_pos;
} fs_in;

layout(location=0) out vec4 output_color;

void main() {

    vec3 color = texture(texture_sampler, fs_in.uv).rgb;

    // ambient
    vec3 ambient = 0.05 * color;
    
    // diffuse
    vec3 light_dir = normalize(fs_in.light_pos - fs_in.position);
    vec3 normal = normalize(fs_in.normals);
    float diff = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = diff * color;

    // specular
    /* vec3 viewDir = normalize(viewPos - fs_in.position); */
    vec3 view_dir = normalize(vec3(0, 0, -3) - fs_in.position);
    vec3 reflect_dir = reflect(-light_dir, normal);

    vec3 halfway_dir = normalize(light_dir + view_dir);  
    float spec = pow(max(dot(normal, halfway_dir), 0.0), 32.0);
    
    spec = pow(max(dot(view_dir, reflect_dir), 0.0), 8.0);
    
    vec3 specular = vec3(0.3) * spec;

    output_color = vec4(ambient + diffuse + specular, 1.0); 
    /* output_color = vec4(out_color, 1.0) * ;  */
    /* output_color = vec4(out_color, 1.0); */
}
