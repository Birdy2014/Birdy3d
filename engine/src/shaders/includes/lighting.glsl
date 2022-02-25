#parameter SHADOW_CASCADE_SIZE 1

#type fragment
struct DirectionalLight {
    bool shadow_enabled;

    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;

    mat4 light_space_matrices[SHADOW_CASCADE_SIZE];
    float shadow_cascade_levels[SHADOW_CASCADE_SIZE];
    sampler2DArrayShadow shadow_map;
};

struct PointLight {
    bool shadow_enabled;

    vec3 position;

    vec3 ambient;
    vec3 diffuse;

    float linear;
    float quadratic;

    float far;

    samplerCubeShadow shadow_map;
};

struct Spotlight {
    bool shadow_enabled;

    vec3 position;
    vec3 direction;
    float inner_cutoff;
    float outer_cutoff;

    vec3 ambient;
    vec3 diffuse;

    float linear;
    float quadratic;

    mat4 light_space_matrix;
    sampler2DShadow shadow_map;
};

#parameter DIRECTIONAL_LIGHTS_AMOUNT 0
#parameter POINTLIGHTS_AMOUNT 0
#parameter SPOTLIGHTS_AMOUNT 0

#if DIRECTIONAL_LIGHTS_AMOUNT > 0
uniform DirectionalLight directional_lights[DIRECTIONAL_LIGHTS_AMOUNT];
#endif

#if POINTLIGHTS_AMOUNT > 0
uniform PointLight point_lights[POINTLIGHTS_AMOUNT];
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

vec3 calc_directional_light(DirectionalLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, vec3 material_color, float shininess, float ambient_occlusion, mat4 view) {
    vec3 light_dir = normalize(-light.direction);

    // ambient lighting
    vec3 ambient = light.ambient * material_color * ambient_occlusion;

    // diffuse lighting
    float diff = max(dot(normal, light_dir), 0.0f);
    vec3 diffuse = diff * light.diffuse * material_color;

    // specular lighting
    float spec = calc_specular_factor(normal, light_dir, view_dir, shininess);
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
            vec4 frag_pos_light_space = light.light_space_matrix * vec4(frag_pos + vec3(x, y, 0.0f) * 0.001, 1.0);
            vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
            proj_coords = proj_coords * 0.5f + 0.5f;
            shadow += texture(light.shadow_map, proj_coords, bias);
        }
    }
    shadow /= 9.0;
    */

    vec4 frag_pos_view_space = view * vec4(frag_pos, 1.0);
    float depth = abs(frag_pos_view_space.z);

    int layer = -1;
    for (int i = 0; i < SHADOW_CASCADE_SIZE; ++i) {
        if (depth < light.shadow_cascade_levels[i]) {
            layer = i;
            break;
        }
    }
    if (layer == -1) {
        layer = SHADOW_CASCADE_SIZE - 1;
    }

    vec4 frag_pos_light_space = light.light_space_matrices[layer] * vec4(frag_pos, 1.0);

    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    // float current_depth = proj_coords.z;
    // if (current_depth > 1.0) {
    //     return 0.0;
    // }
    // calculate bias (based on depth map resolution and slope)
    float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);
    bias *= 1 / (light.shadow_cascade_levels[layer] * 0.5f);

    // float shadow = texture(light.shadow_map, vec4(proj_coords, layer), bias);
    float shadow = texture(light.shadow_map, vec4(proj_coords.xy, layer, proj_coords.z));

    return lighting * shadow + ambient;
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, vec3 material_color, float shininess, float ambient_occlusion) {
    vec3 light_dir = normalize(light.position - frag_pos);
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));

    // ambient lighting
    vec3 ambient = light.ambient * material_color * ambient_occlusion * attenuation;

    // diffuse lighting
    float diff = max(dot(normal, light_dir), 0.0f);
    vec3 diffuse = diff * light.diffuse * material_color * attenuation;

    // specular lighting
    float spec = calc_specular_factor(normal, light_dir, view_dir, shininess);
    vec3 specular = light.diffuse * spec;

    vec3 lighting = diffuse + specular;

    if (!light.shadow_enabled)
        return lighting + ambient;

    // SHADOW
    vec3 frag_to_light = frag_pos - light.position;
    float depth = length(frag_to_light) / light.far;
    float bias = 0.0f;
    float shadow = texture(light.shadow_map, vec4(frag_to_light, depth), bias);

    return lighting * shadow + ambient;
}

vec3 calc_spotlight(Spotlight light, vec3 normal, vec3 frag_pos, vec3 view_dir, vec3 material_color, float shininess, float ambient_occlusion) {
    vec3 light_dir = normalize(light.position - frag_pos);
    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon = light.inner_cutoff - light.outer_cutoff;
    float intensity = smoothstep(0.0f, 1.0f, (theta - light.outer_cutoff) / epsilon);

    float distance = length(light.position - frag_pos);
    float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));

    // ambient lighting
    vec3 ambient = light.ambient * material_color * ambient_occlusion * attenuation;

    // diffuse lighting
    float diff = max(dot(normal, light_dir), 0.0f);
    vec3 diffuse = light.diffuse * material_color * diff * intensity * attenuation;

    // specular lighting
    float spec = calc_specular_factor(normal, light_dir, view_dir, shininess);
    vec3 specular = light.diffuse * spec * intensity * attenuation;

    vec3 lighting = diffuse + specular;

    if (!light.shadow_enabled)
        return lighting + ambient;

    // SHADOW
    vec4 frag_pos_light_space = light.light_space_matrix * vec4(frag_pos, 1.0f);
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    proj_coords = proj_coords * 0.5f + 0.5f;
    light_dir = normalize(light.position - frag_pos);
    float bias = 0.0f;
    float shadow = texture(light.shadow_map, proj_coords, bias);

    return lighting * shadow + ambient;
}

vec3 calc_lights(mat4 view, vec3 normal, vec3 frag_pos, vec3 view_dir, vec3 material_color, float shininess, float ambient_occlusion) {
    vec3 lighting = vec3(0);
#if DIRECTIONAL_LIGHTS_AMOUNT > 0
    for (int i = 0; i < DIRECTIONAL_LIGHTS_AMOUNT; i++)
        lighting += calc_directional_light(directional_lights[i], normal, frag_pos, view_dir, material_color, shininess, ambient_occlusion, view);
#endif

#if POINTLIGHTS_AMOUNT > 0
    for (int i = 0; i < POINTLIGHTS_AMOUNT; i++)
        lighting += calc_point_light(point_lights[i], normal, frag_pos, view_dir, material_color, shininess, ambient_occlusion);
#endif

#if SPOTLIGHTS_AMOUNT > 0
    for (int i = 0; i < SPOTLIGHTS_AMOUNT; i++)
        lighting += calc_spotlight(spotlights[i], normal, frag_pos, view_dir, material_color, shininess, ambient_occlusion);
#endif

    return lighting;
}
