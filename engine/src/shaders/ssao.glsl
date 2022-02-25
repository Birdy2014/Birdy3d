#type vertex
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_coords;

out vec2 v_tex_coords;

void main() {
    gl_Position = vec4(in_pos, 1.0f);
    v_tex_coords = in_tex_coords;
}

#type fragment
out float occlusion;

in vec2 v_tex_coords;

uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;
uniform sampler2D tex_noise;

#define KERNEL_SIZE 16

uniform vec3 samples[KERNEL_SIZE];

// parameters
float radius = 0.07;
float bias = 0.01;

uniform mat4 projection;
uniform mat4 view;

void main() {
    // get input for SSAO algorithm
    vec3 frag_pos = texture(gbuffer_position, v_tex_coords).xyz;
    vec3 normal = texture(gbuffer_normal, v_tex_coords).xyz;
    vec3 random_vector = texture(tex_noise, v_tex_coords * (textureSize(gbuffer_position, 0) / textureSize(tex_noise, 0))).xyz;
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(random_vector - normal * dot(random_vector, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float position_depth = (view * vec4(frag_pos, 1.0)).z;
    occlusion = 1.0;
    for(int i = 0; i < KERNEL_SIZE; i++) {
        // get sample position
        vec4 sample_pos = view * vec4(frag_pos + TBN * samples[i] * radius, 1);

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 projection_coordinates = projection * sample_pos;
        projection_coordinates.xyz = (projection_coordinates.xyz / projection_coordinates.w) * 0.5 + 0.5;

        // get sample depth
        float sample_depth = (view * vec4(texture(gbuffer_position, projection_coordinates.xy).xyz, 1.0)).z;

        // range check & accumulate
        float range_check = smoothstep(0.0, 1.0, radius / abs(position_depth - sample_depth));
        occlusion -= (sample_depth >= sample_pos.z + bias ? 1.0 : 0.0) * (range_check / KERNEL_SIZE);
    }
}
