#parameter TEXTURE

#type vertex
layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_tex;

out vec2 v_frag_tex;
out vec2 v_position;

uniform mat4 projection;
uniform mat4 transform;

void main() {
    v_position = vec2(transform * vec4(in_pos, 0.0f, 1.0f));
    gl_Position = projection * vec4(v_position, 0.0f, 1.0f);
    v_frag_tex = in_tex;
}

#type fragment
out vec4 frag_color;

in vec2 v_frag_tex;
in vec2 v_position;

#if TEXTURE == 1
uniform sampler2D rect_texture;
#else
uniform vec4 fill_color;
uniform vec4 outline_color;
uniform int outline_width;
uniform mat4 transform;
#endif

void main() {
#if TEXTURE == 1
    frag_color = texture(rect_texture, v_frag_tex);
#else
    vec2 inner_top_left = vec2(transform * vec4(0.0f, 0.0f, 0.0f, 1.0f)) + vec2(outline_width);
    vec2 inner_bottom_right = vec2(transform * vec4(1.0f)) - vec2(outline_width);

    if (v_position.x < inner_top_left.x || v_position.y < inner_top_left.y || v_position.x > inner_bottom_right.x || v_position.y > inner_bottom_right.y) {
        frag_color = outline_color;
    } else {
        frag_color = fill_color;
    }
#endif
}
