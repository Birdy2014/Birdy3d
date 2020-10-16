#version 330 core
out vec4 FragColor;

struct Light {
	int type;       // 0: directional; 1: point; 2: spotlight
    vec3 position;
	vec3 direction;
	float innerCutOff;   // for spotlight
	float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float linear;
	float quadratic;

	mat4 lightSpaceMatrix;
	sampler2D shadowMap;
};

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 viewPos;
uniform Light lights[10];
uniform int nrLights;
//uniform int ambientCount;
	
vec3 calcLightColor(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float materialSpecular) {
	vec3 lightDir;
	switch (light.type) {
		case 0: { // directional
			lightDir = normalize(-light.direction);
			break;
		}
		case 1: { // point
			lightDir = normalize(light.position - fragPos);
			float distance = length(light.position - fragPos);
			float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));
			//light.ambient *= attenuation;
			light.diffuse *= attenuation;
			light.specular *= attenuation;
			break;
		}
		case 2: { // spotlight
			lightDir = normalize(light.position - fragPos);
			float theta = dot(lightDir, normalize(-light.direction));
			float epsilon = light.innerCutOff - light.outerCutOff;
			float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
			float distance = length(light.position - fragPos);
			float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));
			//light.ambient *= attenuation * intensity;
			light.diffuse *= attenuation * intensity;
			light.specular *= attenuation * intensity;
			break;
		}
		default:
			lightDir = normalize(-light.direction);
	}

	// ambient lighting
	//vec3 ambient = light.ambient * materialColor;

	// diffuse lighting
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * materialColor;

	// specular lighting
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float shininess = 16;
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	vec3 specular = light.specular * spec * materialSpecular;

	return diffuse + specular;
}

float calcShadow(vec3 FragPos, mat4 lightSpaceMatrix, sampler2D shadowMap, vec3 lightPos) {
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

void main() {
	vec3 FragPos = texture(gPosition, TexCoord).rgb;
	vec3 Normal = texture(gNormal, TexCoord).rgb;
	vec3 Diffuse = texture(gAlbedoSpec, TexCoord).rgb;
	vec3 viewDir = normalize(viewPos - FragPos);
	float specular = texture(gAlbedoSpec, TexCoord).a;

	vec3 lighting = Diffuse * 0.1;
	for (int i = 0; i < nrLights; i++) {
		lighting += calcLightColor(lights[i], Normal, FragPos, viewDir, Diffuse, specular) * calcShadow(FragPos, lights[i].lightSpaceMatrix, lights[i].shadowMap, lights[i].position);
	}

	FragColor = vec4(lighting, 1.0);
}