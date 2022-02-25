#type vertex
layout (location = 0) in vec3 in_pos;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(in_pos, 1.0);
}

#type fragment
out vec4 frag_color;

uniform vec4 color;

void main() {
    frag_color = color;
}
