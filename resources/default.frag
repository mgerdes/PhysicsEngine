#version 450
in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_tex_coord;

layout(location = 3) uniform vec3 ambient;
layout(location = 4) uniform vec3 diffuse;
layout(location = 5) uniform vec3 specular;
layout(location = 6) uniform float shininess;

layout(binding = 0) uniform sampler2D diffuse_map;

out vec4 color;

void main () {
    float kd = max(0.0, dot(normalize(vec3(1.0, 1.0, 1.0)), normalize(frag_normal)));
    vec3 diffuse_color = kd * texture(diffuse_map, frag_tex_coord).rgb;
    vec3 ambient_color = 0.1 * ambient;
    color = vec4(diffuse_color + ambient_color, 1.0);
}
