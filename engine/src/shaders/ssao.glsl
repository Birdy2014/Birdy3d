#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 texture_coordinates;

void main() {
    gl_Position = vec4(aPos, 1.0);
    texture_coordinates = aTexCoords;
}

#type fragment
#version 330 core
out float occlusion;

in vec2 texture_coordinates;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

#define KERNEL_SIZE 16

uniform vec3 samples[KERNEL_SIZE];

// parameters
float radius = 0.07;
float bias = 0.01;

uniform mat4 projection;
uniform mat4 view;

void main() {
    // get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, texture_coordinates).xyz;
    vec3 normal = texture(gNormal, texture_coordinates).xyz;
    vec3 randomVec = texture(texNoise, texture_coordinates * (textureSize(gPosition, 0) / textureSize(texNoise, 0))).xyz;
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float position_depth = (view * vec4(fragPos, 1.0)).z;
    occlusion = 1.0;
    for(int i = 0; i < KERNEL_SIZE; i++) {
        // get sample position
        vec4 sample_pos = view * vec4(fragPos + TBN * samples[i] * radius, 1);

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 projection_coordinates = projection * sample_pos;
        projection_coordinates.xyz = (projection_coordinates.xyz / projection_coordinates.w) * 0.5 + 0.5;

        // get sample depth
        float sample_depth = (view * vec4(texture(gPosition, projection_coordinates.xy).xyz, 1.0)).z;

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position_depth - sample_depth));
        occlusion -= (sample_depth >= sample_pos.z + bias ? 1.0 : 0.0) * (rangeCheck / KERNEL_SIZE);
    }
}
