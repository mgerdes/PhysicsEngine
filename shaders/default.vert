layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_tex_coord;

layout(location = MODEL_MAT_LOCATION) uniform mat4 model_mat;
layout(location = VIEW_MAT_LOCATION) uniform mat4 view_mat;
layout(location = PROJ_MAT_LOCATION) uniform mat4 proj_mat;
layout(location = FOR_UI_LOCATION) uniform bool for_ui;

out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_tex_coord;

void main () {
    if (for_ui) {
        gl_Position = vec4(vertex_position, 1.0);
        frag_position = vec4(vertex_position, 1.0).xyz;
        frag_normal = vec3(0.0, 0.0, 1.0);
        frag_tex_coord = vertex_tex_coord;
    }
    else {
        gl_Position = proj_mat * view_mat * model_mat * vec4(vertex_position, 1.0);
        frag_position = (model_mat * vec4(vertex_position, 1.0)).xyz;
        frag_normal = (inverse(transpose(model_mat)) * vec4(vertex_normal, 1.0)).xyz;
        frag_tex_coord = vertex_tex_coord;
    }
}
