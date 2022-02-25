#type vertex
layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_tex;
layout (location = 2) in vec4 in_color;

out vec2 v_frag_tex;
out vec4 v_color;

uniform mat4 projection;
uniform mat4 move;
uniform mat4 move_self;

void main() {
    gl_Position = projection * (move * move_self * vec4(in_pos, 0, 1));
    v_frag_tex = in_tex;
    v_color = in_color;
}

#type fragment
out vec4 frag_color;

in vec2 v_frag_tex;
in vec4 v_color;

uniform sampler2D font_atlas;

void main() {
    frag_color = vec4(v_color.rgb, texture(font_atlas, v_frag_tex).r * v_color.a);
}
