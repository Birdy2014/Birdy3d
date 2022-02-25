#type vertex
layout (location = 0) in vec3 in_pos;

uniform mat4 light_space_matrix;
uniform mat4 model;

void main() {
    gl_Position = light_space_matrix * model * vec4(in_pos, 1.0);
}

#type fragment

out vec4 frag_color;

void main() {

}
