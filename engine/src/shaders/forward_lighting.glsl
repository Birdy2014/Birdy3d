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

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform bool useTexture;
uniform vec4 color;
uniform float specular;
uniform vec3 viewPos;

#include includes/lighting

void main() {
    vec4 var_diffuse;
    float var_specular;
    vec3 var_normal;
    vec3 viewDir = normalize(viewPos - FragPos);
    if (useTexture) {
        var_diffuse = texture(texture_diffuse1, TexCoords).rgba;
        var_specular = texture(texture_specular1, TexCoords).r;
        var_normal = normalize(TBN * texture(texture_normal1, TexCoords).rgb * 2.0 - 1.0);
    } else {
        var_diffuse = color;
        var_specular = specular;
        var_normal = normalize(Normal);
    }
    if (var_diffuse.a < 0.1)
        discard;

    vec3 lighting = calcLights(var_normal, FragPos, viewDir, var_diffuse.rgb, var_specular);

    FragColor = vec4(lighting, var_diffuse.a);
}
