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

struct DirectionalLight {
    vec3 position;
	vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;

	mat4 lightSpaceMatrix;
	sampler2D shadowMap;
};

struct PointLight {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;

	float linear;
	float quadratic;

	float far;

	samplerCube shadowMap;
};

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 viewPos;

#define NR_DIRECTIONAL_LIGHS 1
#define NR_POINT_LIGHTS 1
uniform DirectionalLight dirLights[NR_DIRECTIONAL_LIGHS];
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float materialSpecular);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float materialSpecular);
float calcDirShadow(vec3 FragPos, mat4 lightSpaceMatrix, sampler2D shadowMap, vec3 lightPos);
float calcPointShadow(vec3 fragPos, samplerCube shadowMap, vec3 lightPos, float far);

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float materialSpecular)
{
	vec3 lightDir = normalize(-light.direction);

	// ambient lighting
	//vec3 ambient = light.ambient * materialColor;

	// diffuse lighting
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * materialColor;

	// specular lighting
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float shininess = 16;
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	vec3 specular = light.diffuse * spec * materialSpecular;

	return diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float materialSpecular)
{
	vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));
    //light.ambient *= attenuation;
    light.diffuse *= attenuation;

	// ambient lighting
	//vec3 ambient = light.ambient * materialColor;

	// diffuse lighting
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * materialColor;

	// specular lighting
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float shininess = 16;
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	vec3 specular = light.diffuse * spec * materialSpecular;

	return diffuse + specular;
}

/* SHADOWS */
float calcDirShadow(vec3 FragPos, mat4 lightSpaceMatrix, sampler2D shadowMap, vec3 lightPos)
{
	vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	vec3 normal = texture(gNormal, TexCoord).rgb;
	vec3 lightDir = normalize(lightPos - texture(gPosition, TexCoord).rgb);
	//float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	float bias = 0;

	float shadow = 0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	if (projCoords.z > 1.0)
		shadow = 0.0;
	return 1 - shadow;
}

float calcPointShadow(vec3 fragPos, samplerCube shadowMap, vec3 lightPos, float far)
{
	vec3 fragToLight = fragPos - lightPos;
	float currentDepth = length(fragToLight);
	float bias = 0.01;
	float closestDepth = texture(shadowMap, fragToLight).r;
	closestDepth *= far;
	float shadow = (currentDepth - bias < closestDepth) ? 1.0 : 0.0;

	return shadow;
}

void main()
{
	vec3 fragPos = texture(gPosition, TexCoord).rgb;
	vec3 normal = texture(gNormal, TexCoord).rgb;
	vec3 diffuse = texture(gAlbedoSpec, TexCoord).rgb;
	float specular = texture(gAlbedoSpec, TexCoord).a;
	vec3 viewDir = normalize(viewPos - fragPos);

    vec3 lighting = vec3(0);
    for (int i = 0; i < NR_DIRECTIONAL_LIGHS; i++)
        lighting += calcDirLight(dirLights[i], normal, fragPos, viewDir, diffuse, specular) * calcDirShadow(fragPos, dirLights[i].lightSpaceMatrix, dirLights[i].shadowMap, dirLights[i].position);
    
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        lighting += calcPointLight(pointLights[i], normal, fragPos, viewDir, diffuse, specular) * calcPointShadow(fragPos, pointLights[i].shadowMap, pointLights[i].position, pointLights[i].far);

	FragColor = vec4(lighting, 1.0);
}
