#include includes/geometry_vertex_shader.glsl
#include includes/material.glsl

#type fragment
layout (location = 0) out vec3 gbuffer_position;
layout (location = 1) out vec3 gbuffer_normal;
layout (location = 2) out vec4 gbuffer_albedo_spec;

in vec2 v_tex_coords;
in vec3 v_frag_pos;
in vec3 v_normal;
in mat3 TBN;

void main() {
    gbuffer_position = v_frag_pos;
    gbuffer_albedo_spec.rgb = material.diffuse_map_enabled ? texture(material.diffuse_map, v_tex_coords).rgb : material.diffuse_color.rgb;

    if (material.normal_map_enabled)
        gbuffer_normal = normalize(TBN * (texture(material.normal_map, v_tex_coords).rgb * 2.0 - 1.0));
    else
        gbuffer_normal = normalize(v_normal);

    gbuffer_albedo_spec.a = material.specular_map_enabled ? texture(material.specular_map, v_tex_coords).r : material.specular_value / 100;
}
