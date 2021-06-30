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
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

#include includes/material.glsl

void main() {
    gPosition = FragPos;
    gAlbedoSpec.rgb = material.diffuse_map_enabled ? texture(material.diffuse_map, TexCoords).rgb : material.diffuse_color.rgb;

    if (material.normal_map_enabled)
        gNormal = normalize(TBN * texture(material.normal_map, TexCoords).rgb * 2.0 - 1.0);
    else
        gNormal = normalize(Normal);

    gAlbedoSpec.a = material.specular_map_enabled ? texture(material.specular_map, TexCoords).r : material.specular_value;
}
