#type vertex
#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;

out vec2 FragTex;

uniform bool hasMatrix;
uniform mat4 move;

void main() {
    if (hasMatrix)
        gl_Position = move * vec4(pos, 0, 1);
    else
        gl_Position = vec4(pos, 0, 1);
    FragTex = tex;
}

#type fragment
#version 330 core
out vec4 FragColor;

in vec2 FragTex;

uniform bool isText;
uniform bool hasTexture;
uniform sampler2D rectTexture;
uniform vec4 color;

void main() {
    if (isText)
        FragColor = vec4(color.rgb, texture(rectTexture, FragTex).r * color.a);
    else if (hasTexture)
        FragColor = texture(rectTexture, FragTex);
    else
        FragColor = color;
}
