#type vertex
layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_tex;

out vec2 v_frag_tex;

uniform mat4 projection;
uniform mat4 move;
uniform mat4 move_self;

void main() {
    gl_Position = projection * (move * move_self * vec4(in_pos, 0, 1));
    v_frag_tex = in_tex;
}

#type fragment
out vec4 frag_color;

in vec2 v_frag_tex;

uniform int type; // 0 - FILLED, 1 - OUTLINE, 2 - TEXTURE
uniform sampler2D rect_texture;
uniform vec4 color;

void main() {
    if (type == 2)
        frag_color = texture(rect_texture, v_frag_tex);
    else
        frag_color = color;
}
