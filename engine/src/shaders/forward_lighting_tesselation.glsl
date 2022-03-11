#include includes/lighting.glsl
#include includes/material.glsl
#include includes/tesselation.glsl

#type fragment
out vec4 frag_color;

in vec3 te_frag_pos;
in vec2 te_tex_coords;
in vec3 te_normal;
in mat3 te_tbn_matrix;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform vec3 view_pos;
uniform mat4 view;
uniform mat4 model;

void main() {
    vec3 x = dFdx(te_frag_pos);
    vec3 y = dFdy(te_frag_pos);
    vec3 normal = inverse(mat3(model)) * normalize(cross(x, y));

    vec3 view_dir = normalize(view_pos - te_frag_pos);
    vec4 var_diffuse = material.diffuse_map_enabled ? texture(material.diffuse_map, te_tex_coords).rgba : material.diffuse_color;
    float var_specular = material.specular_map_enabled ? texture(material.specular_map, te_tex_coords).r * 100 : material.specular_value;
    vec3 var_normal = material.normal_map_enabled ? normalize(te_tbn_matrix * (texture(material.normal_map, te_tex_coords).rgb * 2.0 - 1.0)) : normal;
    if (var_diffuse.a < 0.1)
        discard;

    vec3 lighting = calc_lights(view, var_normal, te_frag_pos, view_dir, var_diffuse.rgb, var_specular, 1.0f);

    frag_color = vec4(lighting, var_diffuse.a);
}
