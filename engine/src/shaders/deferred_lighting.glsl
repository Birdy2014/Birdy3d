#include includes/lighting.glsl

#type vertex
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}

#type fragment
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

uniform vec3 viewPos;

void main() {
    vec3 fragPos = texture(gPosition, TexCoord).rgb;
    vec3 normal = texture(gNormal, TexCoord).rgb;
    vec3 diffuse = texture(gAlbedoSpec, TexCoord).rgb;
    float specular = texture(gAlbedoSpec, TexCoord).a * 100.0f;
    vec3 viewDir = normalize(viewPos - fragPos);
    float ambient_occlusion = texture(ssao, TexCoord).r;

    vec3 lighting = calcLights(normal, fragPos, viewDir, diffuse, specular, ambient_occlusion);

    FragColor = vec4(lighting, 1.0f);
}
