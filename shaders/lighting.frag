#version 330 core
out vec4 FragColor;

struct Material {
    float shininess;
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
	sampler2D texture_emissive1;

	// for material without texture
	vec4 color;
	vec3 specular;
	vec3 emissive;
}; 

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

	sampler2D shadowMap;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace[2];

const int NR_LIGHTS = 16;

uniform bool useTexture;
uniform bool hasSpecular;
uniform bool hasEmissive;
uniform vec3 viewPos;
uniform Material material;
uniform Light lights[NR_LIGHTS];
	
vec3 calcLightColor(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, vec3 materialSpecular) {
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
			lightDir = normalize(light.direction);
	}

	// ambient lighting
	//vec3 ambient = light.ambient * materialColor;

	// diffuse lighting
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * materialColor;

	// specular lighting
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = materialSpecular * spec * light.specular;

	return (diffuse + specular);
}

float calcShadow(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 lightPos) {
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

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
	vec4 baseColor;
	vec3 specular = vec3(1.0);
	vec3 emission = vec3(0.0);
	if (useTexture) {
		baseColor = texture(material.texture_diffuse1, TexCoord).rgba;
		if (hasSpecular) specular = texture(material.texture_specular1, TexCoord).rgb;
		if (hasEmissive) emission = texture(material.texture_emissive1, TexCoord).rgb;
	} else {
		baseColor = material.color;
		if (hasSpecular) specular = material.specular;
		if (hasEmissive) emission = material.emissive;
	}

	// transparency
	if (baseColor.a < 0.1) { //FIXME: some textures don't have alpha, so it wrongly assumes 0.0
		//discard;
	}
	//TODO: partial transparency

	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = vec3(0.0);
	vec3 totalAmbient = vec3(0.0);
	int ambientCount = 0;
	for (int i = 0; i < NR_LIGHTS; i++) {
		result += calcLightColor(lights[i], norm, FragPos, viewDir, baseColor.rgb, specular) * calcShadow(FragPosLightSpace[i], lights[i].shadowMap, lights[i].position);
		if (lights[i].ambient != vec3(0.0)) {
			totalAmbient += lights[i].ambient;
			ambientCount++;
		}
	}

	// result
	FragColor = vec4(result + emission + totalAmbient / ambientCount, 1.0);
}