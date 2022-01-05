#type fragment
struct Material {
    bool diffuse_map_enabled;
    vec4 diffuse_color;
    sampler2D diffuse_map;

    bool specular_map_enabled;
    float specular_value;
    sampler2D specular_map;

    bool normal_map_enabled;
    sampler2D normal_map;

    bool emissive_map_enabled;
    vec4 emissive_color;
    sampler2D emissive_map;
};

uniform Material material;
