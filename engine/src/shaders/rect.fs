#version 330 core
out vec4 FragColor;

in vec3 FragPos;
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
