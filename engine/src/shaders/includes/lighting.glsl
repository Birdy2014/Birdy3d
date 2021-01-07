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

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float shininess)
{
	vec3 lightDir = normalize(-light.direction);

	// ambient lighting
	//vec3 ambient = light.ambient * materialColor;

	// diffuse lighting
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * materialColor;

	// specular lighting
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess * 32) * shininess;
	vec3 specular = light.diffuse * spec;

	vec3 lighting = diffuse + specular;

	// SHADOW
	vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(fragPos, 1.0);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(light.shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	lightDir = normalize(light.position - fragPos);
	//float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	float bias = 0;

	float shadow = 0;
	vec2 texelSize = 1.0 / textureSize(light.shadowMap, 0);
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			float pcfDepth = texture(light.shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	if (projCoords.z > 1.0)
		shadow = 0.0;
	return lighting * (1 - shadow);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float shininess)
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
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess * 32) * shininess;
	vec3 specular = light.diffuse * spec;

	vec3 lighting = diffuse + specular;

	// SHADOW
	vec3 fragToLight = fragPos - light.position;
	float currentDepth = length(fragToLight);
	float bias = 0.00;
	float closestDepth = texture(light.shadowMap, fragToLight).r;
	closestDepth *= light.far;
	float shadow = (currentDepth - bias < closestDepth) ? 1.0 : 0.0;

	return lighting * shadow;
}

/* SHADOWS */
float calcDirShadow(vec3 fragPos, mat4 lightSpaceMatrix, sampler2D shadowMap, vec3 lightPos, vec3 normal)
{
	vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	vec3 lightDir = normalize(lightPos - fragPos);
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