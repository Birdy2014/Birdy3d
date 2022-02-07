#parameter SHADOW_CASCADE_SIZE 1

#type fragment
struct DirectionalLight {
    bool shadow_enabled;

    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;

    mat4 lightSpaceMatrices[SHADOW_CASCADE_SIZE];
    float shadow_cascade_levels[SHADOW_CASCADE_SIZE];
    sampler2DArrayShadow shadowMap;
};

struct PointLight {
    bool shadow_enabled;

    vec3 position;

    vec3 ambient;
    vec3 diffuse;

    float linear;
    float quadratic;

    float far;

    samplerCubeShadow shadowMap;
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
    sampler2DShadow shadowMap;
};

#parameter DIRECTIONAL_LIGHTS_AMOUNT 0
#parameter POINTLIGHTS_AMOUNT 0
#parameter SPOTLIGHTS_AMOUNT 0

#if DIRECTIONAL_LIGHTS_AMOUNT > 0
uniform DirectionalLight dirLights[DIRECTIONAL_LIGHTS_AMOUNT];
#endif

#if POINTLIGHTS_AMOUNT > 0
uniform PointLight pointLights[POINTLIGHTS_AMOUNT];
#endif

#if SPOTLIGHTS_AMOUNT > 0
uniform Spotlight spotlights[SPOTLIGHTS_AMOUNT];
#endif

float calc_specular_factor(vec3 normal, vec3 light_dir, vec3 view_dir, float shininess) {
    vec3 halfwayDir = normalize(light_dir + view_dir);
    if (shininess <= 0.0f)
        return 0.0f;
    return pow(max(dot(normal, halfwayDir), 0.0f), shininess) * (shininess / 100.0f);
}

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float shininess, float ambient_occlusion, mat4 view) {
    vec3 lightDir = normalize(-light.direction);

    // ambient lighting
    vec3 ambient = light.ambient * materialColor * ambient_occlusion;

    // diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * light.diffuse * materialColor;

    // specular lighting
    float spec = calc_specular_factor(normal, lightDir, viewDir, shininess);
    vec3 specular = light.diffuse * spec;

    vec3 lighting = diffuse + specular;

    if (!light.shadow_enabled)
        return lighting + ambient;

    // SHADOW
    /*
    float bias = 0.0f;

    float shadow = 0.0;
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(fragPos + vec3(x, y, 0.0f) * 0.001, 1.0);
            vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
            projCoords = projCoords * 0.5f + 0.5f;
            shadow += texture(light.shadowMap, projCoords, bias);
        }
    }
    shadow /= 9.0;
    */

    vec4 fragPosViewSpace = view * vec4(fragPos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < SHADOW_CASCADE_SIZE; ++i) {
        if (depthValue < light.shadow_cascade_levels[i]) {
            layer = i;
            break;
        }
    }
    if (layer == -1) {
        layer = SHADOW_CASCADE_SIZE - 1;
    }

    vec4 fragPosLightSpace = light.lightSpaceMatrices[layer] * vec4(fragPos, 1.0);

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    // float currentDepth = projCoords.z;
    // if (currentDepth > 1.0) {
    //     return 0.0;
    // }
    // calculate bias (based on depth map resolution and slope)
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    bias *= 1 / (light.shadow_cascade_levels[layer] * 0.5f);

    // float shadow = texture(light.shadowMap, vec4(projCoords, layer), bias);
    float shadow = texture(light.shadowMap, vec4(projCoords.xy, layer, projCoords.z));

    return lighting * shadow + ambient;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float shininess, float ambient_occlusion) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));

    // ambient lighting
    vec3 ambient = light.ambient * materialColor * ambient_occlusion * attenuation;

    // diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * light.diffuse * materialColor * attenuation;

    // specular lighting
    float spec = calc_specular_factor(normal, lightDir, viewDir, shininess);
    vec3 specular = light.diffuse * spec;

    vec3 lighting = diffuse + specular;

    if (!light.shadow_enabled)
        return lighting + ambient;

    // SHADOW
    vec3 fragToLight = fragPos - light.position;
    float currentDepth = length(fragToLight) / light.far;
    float bias = 0.0f;
    float shadow = texture(light.shadowMap, vec4(fragToLight, currentDepth), bias);

    return lighting * shadow + ambient;
}

vec3 calcSpotlight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float shininess, float ambient_occlusion) {
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = smoothstep(0.0f, 1.0f, (theta - light.outerCutOff) / epsilon);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));

    // ambient lighting
    vec3 ambient = light.ambient * materialColor * ambient_occlusion * attenuation;

    // diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * materialColor * diff * intensity * attenuation;

    // specular lighting
    float spec = calc_specular_factor(normal, lightDir, viewDir, shininess);
    vec3 specular = light.diffuse * spec * intensity * attenuation;

    vec3 lighting = diffuse + specular;

    if (!light.shadow_enabled)
        return lighting + ambient;

    // SHADOW
    vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(fragPos, 1.0f);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5f + 0.5f;
    lightDir = normalize(light.position - fragPos);
    float bias = 0.0f;
    float shadow = texture(light.shadowMap, projCoords, bias);

    return lighting * shadow + ambient;
}

vec3 calcLights(mat4 view, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float shininess, float ambient_occlusion) {
    vec3 lighting = vec3(0);
#if DIRECTIONAL_LIGHTS_AMOUNT > 0
    for (int i = 0; i < DIRECTIONAL_LIGHTS_AMOUNT; i++)
        lighting += calcDirLight(dirLights[i], normal, fragPos, viewDir, materialColor, shininess, ambient_occlusion, view);
#endif

#if POINTLIGHTS_AMOUNT > 0
    for (int i = 0; i < POINTLIGHTS_AMOUNT; i++)
        lighting += calcPointLight(pointLights[i], normal, fragPos, viewDir, materialColor, shininess, ambient_occlusion);
#endif

#if SPOTLIGHTS_AMOUNT > 0
    for (int i = 0; i < SPOTLIGHTS_AMOUNT; i++)
        lighting += calcSpotlight(spotlights[i], normal, fragPos, viewDir, materialColor, shininess, ambient_occlusion);
#endif

    return lighting;
}
