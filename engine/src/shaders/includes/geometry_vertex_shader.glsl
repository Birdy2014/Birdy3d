#type vertex
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coords;
layout (location = 3) in vec3 in_tangent;

out vec3 v_frag_pos;
out vec2 v_tex_coords;
out vec3 v_normal;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 world_pos = model * vec4(in_pos, 1.0f);

    v_frag_pos = world_pos.xyz;
    v_tex_coords = in_tex_coords;

    mat3 normal_matrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normal_matrix * in_tangent);
    v_normal = normalize(normal_matrix * in_normal);
    T = normalize(T - dot(T, v_normal) * v_normal);
    vec3 B = cross(v_normal, T);

    TBN = mat3(T, B, v_normal);

    gl_Position = projection * view * world_pos;
}
