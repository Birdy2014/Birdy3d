#type vertex
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_coords;

out vec2 v_tex_coords;

void main() {
    gl_Position = vec4(in_pos, 1.0);
    v_tex_coords = in_tex_coords;
}

#type fragment
out float frag_color;

in vec2 v_tex_coords;

uniform sampler2D ssao_input;

void main() {
    vec2 texel_size = 1.0 / vec2(textureSize(ssao_input, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            result += texture(ssao_input, v_tex_coords + offset).r;
        }
    }
    frag_color = result / (4.0 * 4.0);
}
