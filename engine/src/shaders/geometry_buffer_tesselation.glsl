#include includes/material.glsl
#include includes/tesselation.glsl

#type fragment
layout (location = 0) out vec3 gbuffer_position;
layout (location = 1) out vec3 gbuffer_normal;
layout (location = 2) out vec4 gbuffer_albedo_spec;

in vec2 te_tex_coords;
in vec3 te_frag_pos;
in vec3 te_normal;
in mat3 te_tbn_matrix;

uniform mat4 model;

void main() {
    vec3 x = dFdx(te_frag_pos);
    vec3 y = dFdy(te_frag_pos);
    vec3 normal = inverse(mat3(model)) * normalize(cross(x, y));

    gbuffer_position = te_frag_pos;
    gbuffer_albedo_spec.rgb = material.diffuse_map_enabled ? texture(material.diffuse_map, te_tex_coords).rgb : material.diffuse_color.rgb;

    if (material.normal_map_enabled)
        gbuffer_normal = normalize(te_tbn_matrix * (texture(material.normal_map, te_tex_coords).rgb * 2.0 - 1.0));
    else
        gbuffer_normal = normalize(normal);

    gbuffer_albedo_spec.a = material.specular_map_enabled ? texture(material.specular_map, te_tex_coords).r : material.specular_value / 100;
}
