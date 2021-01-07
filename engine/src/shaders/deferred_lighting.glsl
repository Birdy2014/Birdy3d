#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}

#type fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 viewPos;

#include includes/lighting
#define NR_DIRECTIONAL_LIGHS 1
#define NR_POINT_LIGHTS 1
uniform DirectionalLight dirLights[NR_DIRECTIONAL_LIGHS];
uniform PointLight pointLights[NR_POINT_LIGHTS];

void main()
{
	vec3 fragPos = texture(gPosition, TexCoord).rgb;
	vec3 normal = texture(gNormal, TexCoord).rgb;
	vec3 diffuse = texture(gAlbedoSpec, TexCoord).rgb;
	float specular = texture(gAlbedoSpec, TexCoord).a;
	vec3 viewDir = normalize(viewPos - fragPos);

    vec3 lighting = vec3(0);
    for (int i = 0; i < NR_DIRECTIONAL_LIGHS; i++)
        lighting += calcDirLight(dirLights[i], normal, fragPos, viewDir, diffuse, specular);
    
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        lighting += calcPointLight(pointLights[i], normal, fragPos, viewDir, diffuse, specular);

	FragColor = vec4(lighting, 1.0);
}
