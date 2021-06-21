struct Material {
    sampler2D diffuse_map;
    sampler2D specular_map;
    bool has_normal_map;
    sampler2D normal_map;
    sampler2D emissive_map;
};

uniform Material material;
