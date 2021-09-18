struct DirectionalLight {
    bool shadow_enabled;

    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;

    mat4 lightSpaceMatrix;
    sampler2D shadowMap;
};

struct PointLight {
    bool shadow_enabled;

    vec3 position;

    vec3 ambient;
    vec3 diffuse;

    float linear;
    float quadratic;

    float far;

    samplerCube shadowMap;
};

struct Spotlight {
    bool shadow_enabled;

    vec3 position;
    vec3 direction;
    float innerCutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;

    float linear;
    float quadratic;

    mat4 lightSpaceMatrix;
    sampler2D shadowMap;
};

uniform DirectionalLight dirLights[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight pointLights[MAX_POINTLIGHTS];
uniform Spotlight spotlights[MAX_SPOTLIGHTS];
uniform int nr_directional_lights;
uniform int nr_pointlights;
uniform int nr_spotlights;

float calc_specular_factor(vec3 normal, vec3 light_dir, vec3 view_dir, float shininess) {
    vec3 halfwayDir = normalize(light_dir + view_dir);
    if (shininess <= 0)
        return 0;
    return pow(max(dot(normal, halfwayDir), 0.0), shininess) * (shininess / 100);
}

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float shininess, float ambient_occlusion) {
    vec3 lightDir = normalize(-light.direction);

    // ambient lighting
    vec3 ambient = light.ambient * materialColor * ambient_occlusion;

    // diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * materialColor;

    // specular lighting
    float spec = calc_specular_factor(normal, lightDir, viewDir, shininess);
    vec3 specular = light.diffuse * spec;

    vec3 lighting = diffuse + specular;

    if (!light.shadow_enabled)
        return lighting + ambient;

    // SHADOW
    vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    //float bias = max(0.0001 * (1.0 - dot(normal, lightDir)), 0.0);
    float bias = 0;

    float shadow = 0;
    vec2 texelSize = 1.0 / textureSize(light.shadowMap, 0);
    texelSize *= 0.4;
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            float pcfDepth = texture(light.shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += ((currentDepth - bias) > pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 25.0;

    if (projCoords.z > 1.0)
        shadow = 0.0;
    return lighting * (1 - shadow) + ambient;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float shininess, float ambient_occlusion) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));

    // ambient lighting
    vec3 ambient = light.ambient * materialColor * ambient_occlusion * attenuation;

    // diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * materialColor * attenuation;

    // specular lighting
    float spec = calc_specular_factor(normal, lightDir, viewDir, shininess);
    vec3 specular = light.diffuse * spec;

    vec3 lighting = diffuse + specular;

    if (!light.shadow_enabled)
        return lighting + ambient;

    // SHADOW
    vec3 fragToLight = fragPos - light.position;
    float currentDepth = length(fragToLight);
    float bias = 0.00;
    float closestDepth = texture(light.shadowMap, fragToLight).r;
    closestDepth *= light.far;
    float shadow = (currentDepth - bias < closestDepth) ? 1.0 : 0.0;

    return lighting * shadow + ambient;
}

vec3 calcSpotlight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float shininess, float ambient_occlusion) {
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = smoothstep(0.0, 1.0, (theta - light.outerCutOff) / epsilon);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));

    // ambient lighting
    vec3 ambient = light.ambient * materialColor * ambient_occlusion * attenuation;

    // diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * materialColor * diff * intensity * attenuation;

    // specular lighting
    float spec = calc_specular_factor(normal, lightDir, viewDir, shininess);
    vec3 specular = light.diffuse * spec * intensity * attenuation;

    vec3 lighting = diffuse + specular;

    if (!light.shadow_enabled)
        return lighting + ambient;

    // SHADOW
    vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(light.shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    lightDir = normalize(light.position - fragPos);
    float bias = 0;
    float shadow = (currentDepth - bias < closestDepth) ? 1.0 : 0.0;

    return lighting * shadow + ambient;
}

vec3 calcLights(vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float shininess, float ambient_occlusion) {
    vec3 lighting = vec3(0);
    for (int i = 0; i < nr_directional_lights; i++)
        lighting += calcDirLight(dirLights[i], normal, fragPos, viewDir, materialColor, shininess, ambient_occlusion);

    for (int i = 0; i < nr_pointlights; i++)
        lighting += calcPointLight(pointLights[i], normal, fragPos, viewDir, materialColor, shininess, ambient_occlusion);

    for (int i = 0; i < nr_spotlights; i++)
        lighting += calcSpotlight(spotlights[i], normal, fragPos, viewDir, materialColor, shininess, ambient_occlusion);

    return lighting;
}
