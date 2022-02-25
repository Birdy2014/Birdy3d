#include includes/lighting.glsl

#type vertex
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_coord;

out vec2 v_tex_coord;

void main() {
    gl_Position = vec4(in_pos, 1.0);
    v_tex_coord = in_tex_coord;
}

#type fragment
out vec4 frag_color;

in vec2 v_tex_coord;

uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;
uniform sampler2D gbuffer_albedo_spec;
uniform sampler2D ssao;

uniform mat4 view;
uniform vec3 view_pos;

void main() {
    vec3 frag_pos = texture(gbuffer_position, v_tex_coord).rgb;
    vec3 normal = texture(gbuffer_normal, v_tex_coord).rgb;
    vec3 diffuse = texture(gbuffer_albedo_spec, v_tex_coord).rgb;
    float specular = texture(gbuffer_albedo_spec, v_tex_coord).a * 100.0f;
    vec3 view_dir = normalize(view_pos - frag_pos);
    float ambient_occlusion = texture(ssao, v_tex_coord).r;

    vec3 lighting = calc_lights(view, normal, frag_pos, view_dir, diffuse, specular, ambient_occlusion);

    frag_color = vec4(lighting, 1.0f);
}
