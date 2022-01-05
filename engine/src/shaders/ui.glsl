#type vertex
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;

out vec2 FragTex;

uniform mat4 projection;
uniform mat4 move;
uniform mat4 move_self;

void main() {
    gl_Position = projection * (move * move_self * vec4(pos, 0, 1));
    FragTex = tex;
}

#type fragment
out vec4 FragColor;

in vec2 FragTex;

uniform int type; // 0 - FILLED, 1 - OUTLINE, 2 - TEXTURE
uniform sampler2D rectTexture;
uniform vec4 color;

void main() {
    if (type == 2)
        FragColor = texture(rectTexture, FragTex);
    else
        FragColor = color;
}
