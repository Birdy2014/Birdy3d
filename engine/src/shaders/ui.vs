#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;

out vec2 FragTex;

uniform bool hasMatrix;
uniform mat4 move;

void main() {
    if (hasMatrix)
        gl_Position = move * vec4(pos, 1);
    else
        gl_Position = vec4(pos, 1);
    FragTex = tex;
}
