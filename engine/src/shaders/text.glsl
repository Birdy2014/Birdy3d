#type vertex
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec4 color_in;

out vec2 frag_tex;
out vec4 color;

uniform mat4 projection;
uniform mat4 move;
uniform mat4 move_self;

void main() {
    gl_Position = projection * (move * move_self * vec4(pos, 0, 1));
    frag_tex = tex;
    color = color_in;
}

#type fragment
out vec4 frag_color;

in vec2 frag_tex;
in vec4 color;

uniform sampler2D font_atlas;

void main() {
    frag_color = vec4(color.rgb, texture(font_atlas, frag_tex).r * color.a);
}
