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

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform bool useTexture;
uniform bool hasNormal;
uniform bool hasSpecular;
uniform vec4 color;
uniform float specular;

void main() {
    gPosition = FragPos;
    if (useTexture)
        gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    else
        gAlbedoSpec.rgb = color.rgb;

    if (useTexture && hasNormal)
        gNormal = normalize(TBN * texture(texture_normal1, TexCoords).rgb * 2.0 - 1.0);
    else
        gNormal = normalize(Normal);

    if (useTexture && hasSpecular)
        gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
    else
        gAlbedoSpec.a = specular;

    if (gAlbedoSpec.a < 0.1)
        discard;
}
