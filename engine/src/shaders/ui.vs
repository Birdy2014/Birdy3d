#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;

out vec2 FragPos;
out vec4 FragCol;

uniform mat4 move;
uniform int width;
uniform int height;

void main() {
    gl_Position = move * vec4((pos.x / width) * 2 - 1, ((pos.y / height) * 2 - 1) * -1, pos.z, 1.0);
    FragPos = vec2(pos);
    FragCol = color;
}
