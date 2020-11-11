#version 330 core
out vec4 FragColor;

in vec2 FragPos;
in vec4 FragCol;

void main() {
    FragColor = FragCol;
}
