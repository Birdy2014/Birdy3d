#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    Normal = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, Normal) * Normal);
    vec3 B = cross(Normal, T);

    TBN = mat3(T, B, Normal);

    gl_Position = projection * view * worldPos;
}

#type fragment
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform vec3 viewPos;

#include includes/lighting
#include includes/material.glsl

void main() {
    vec3 viewDir = normalize(viewPos - FragPos);
    vec4 var_diffuse = material.diffuse_map_enabled ? texture(material.diffuse_map, TexCoords).rgba : material.diffuse_color;
    float var_specular = material.specular_map_enabled ? texture(material.specular_map, TexCoords).r * 255 : material.specular_value;
    vec3 var_normal = material.normal_map_enabled ? normalize(TBN * texture(material.normal_map, TexCoords).rgb * 2.0 - 1.0) : Normal;
    if (var_diffuse.a < 0.1)
        discard;

    vec3 lighting = calcLights(var_normal, FragPos, viewDir, var_diffuse.rgb, var_specular);

    FragColor = vec4(lighting, var_diffuse.a);
}
