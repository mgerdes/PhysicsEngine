#version 450
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_tex_coord;

layout(location = 0) uniform mat4 model_mat;
layout(location = 1) uniform mat4 view_mat;
layout(location = 2) uniform mat4 proj_mat;

out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_tex_coord;

void main () {
    gl_Position = proj_mat * view_mat * model_mat * vec4(vertex_position, 1.0);
    frag_position = (model_mat * vec4(vertex_position, 1.0)).xyz;
    frag_normal = vertex_normal;
    frag_tex_coord = vertex_tex_coord;
}
