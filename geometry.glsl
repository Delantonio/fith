#version 450

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;
   
uniform mat4 projection_matrix;

uniform float size; // Particle size

in vec4 color[];

out vec2 vertex_uv;
out vec4 vertex_color;
   
void main()
{
  vec4 P = gl_in[0].gl_Position;

  vec2 va = P.xy + vec2(-0.5, -0.5) * size;
  gl_Position = projection_matrix * vec4(va.xy, P.zw);
  vertex_uv = vec2(0.0, 0.0);
  vertex_color = vec4(1.0,1.0,1.0,1.0);
  EmitVertex();
  
  vec2 vb = P.xy + vec2(-0.5, 0.5) * size;
  gl_Position = projection_matrix * vec4(vb.xy, P.zw);
  vertex_uv = vec2(0.0, 1.0);
  vertex_color = vec4(1.0,1.0,1.0,1.0);
  EmitVertex();

  vec2 vd = P.xy + vec2(0.5, -0.5) * size;
  gl_Position = projection_matrix * vec4(vd.xy, P.zw);
  vertex_uv = vec2(1.0, 0.0);
  vertex_color = vec4(1.0,1.0,1.0,1.0);
  EmitVertex();

  vec2 vc = P.xy + vec2(0.5, 0.5) * size;
  gl_Position = projection_matrix * vec4(vc.xy, P.zw);
  vertex_uv = vec2(1.0, 1.0);
  vertex_color = vec4(1.0,1.0,1.0,1.0);
  EmitVertex();
  
  EndPrimitive();
}

