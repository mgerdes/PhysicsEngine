in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_tex_coord;

layout(location = AMBIENT_LOCATION) uniform vec3 ambient;
layout(location = DIFFUSE_LOCATION) uniform vec3 diffuse;
layout(location = SPECULAR_LOCATION) uniform vec3 specular;
layout(location = SHININESS_LOCATION) uniform float shininess;
layout(location = SINGLE_COLOR_LOCATION) uniform bool single_color;
layout(location = FOR_SHADOW_LOCATION) uniform bool for_shadow;
layout(location = ONLY_TEXTURE_LOCATION) uniform bool only_texture;

layout(binding = 0) uniform sampler2D diffuse_map;

out vec4 color;

void main () {
    if (for_shadow) {
    }
    else if (single_color) {
        color = vec4(diffuse, 1.0);
    }
    else if (only_texture) {
        color = vec4(texture(diffuse_map, frag_tex_coord).r, 0.0, 0.0, 1.0);
    }
    else {
        float kd = max(0.2, dot(normalize(vec3(1.0, 2.0, 3.0)), normalize(frag_normal)));

        vec3 diffuse_color = 0.9 * kd * texture(diffuse_map, frag_tex_coord).rgb;
        vec3 ambient_color = 0.1 * ambient;

        color = vec4(diffuse_color + ambient_color, 1.0);
    }
}
