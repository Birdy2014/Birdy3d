#version 330 core
out vec4 FragColor;

in vec2 FragPos;
in vec3 FragCol;

void main() {
    FragColor = vec4(FragCol, 0.0);
}