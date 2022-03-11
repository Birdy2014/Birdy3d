// https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation

#type vertex
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coords;
layout (location = 3) in vec3 in_tangent;

out vec2 v_tex_coords;

void main() {
    v_tex_coords = in_tex_coords;

    gl_Position = vec4(in_pos, 1.0f);
}

#type tesselation_control
layout (vertices=4) out;

uniform mat4 model;
uniform mat4 view;

in vec2 v_tex_coords[];

out vec3 tc_frag_pos[];
out vec2 tc_tex_coords[];

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tc_tex_coords[gl_InvocationID] = v_tex_coords[gl_InvocationID];

    if (gl_InvocationID == 0) {
        const int MIN_TESS_LEVEL = 4;
        const int MAX_TESS_LEVEL = 64;
        const float MIN_DISTANCE = 5;
        const float MAX_DISTANCE = 50;

        vec4 eye_space_pos00 = view * model * gl_in[0].gl_Position;
        vec4 eye_space_pos01 = view * model * gl_in[1].gl_Position;
        vec4 eye_space_pos10 = view * model * gl_in[2].gl_Position;
        vec4 eye_space_pos11 = view * model * gl_in[3].gl_Position;

        // ----------------------------------------------------------------------
        // Step 3: "distance" from camera scaled between 0 and 1
        float distance00 = clamp((abs(eye_space_pos00.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float distance01 = clamp((abs(eye_space_pos01.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float distance10 = clamp((abs(eye_space_pos10.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float distance11 = clamp((abs(eye_space_pos11.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);

        // ----------------------------------------------------------------------
        // Step 4: interpolate edge tessellation level based on closer vertex
        float tess_level0 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00) );
        float tess_level1 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01) );
        float tess_level2 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11) );
        float tess_level3 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10) );

        // ----------------------------------------------------------------------
        // Step 5: set the corresponding outer edge tessellation levels
        gl_TessLevelOuter[0] = tess_level0;
        gl_TessLevelOuter[1] = tess_level1;
        gl_TessLevelOuter[2] = tess_level2;
        gl_TessLevelOuter[3] = tess_level3;

        // ----------------------------------------------------------------------
        // Step 6: set the inner tessellation levels to the max of the two parallel edges
        gl_TessLevelInner[0] = max(tess_level1, tess_level3);
        gl_TessLevelInner[1] = max(tess_level0, tess_level2);
    }
}

#type tesselation_evaluation
layout (quads, fractional_odd_spacing, ccw) in;

uniform sampler2D height_map;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 tc_frag_pos[];
in vec2 tc_tex_coords[];

out vec3 te_frag_pos;
out vec2 te_tex_coords;
out vec3 te_normal;
out mat3 te_tbn_matrix;

out float te_height;

void main() {
    const float height_scale = 1;

    // get patch coordinate
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // ----------------------------------------------------------------------
    // retrieve control point texture coordinates
    vec2 t00 = tc_tex_coords[0];
    vec2 t01 = tc_tex_coords[1];
    vec2 t10 = tc_tex_coords[2];
    vec2 t11 = tc_tex_coords[3];

    // bilinearly interpolate texture coordinate across patch
    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    te_tex_coords = (t1 - t0) * v + t0;

    // lookup texel at patch coordinate for height and scale + shift as desired
    te_height = texture(height_map, te_tex_coords).x * height_scale;

    // ----------------------------------------------------------------------
    // retrieve control point position coordinates
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // bilinearly interpolate position coordinate across patch
    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;

    // displace point along normal
    p.y = te_height;

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    gl_Position = projection * view * model * p;

    // world space
    te_frag_pos = (model * p).xyz;

    // compute normal
    //vec2 texel_size = 1.0f / textureSize(height_map, 0);

    //float left  = texture(height_map, te_tex_coords + vec2(-texel_size.x, 0.0)).r * height_scale * 2.0 - 1.0;
    //float right = texture(height_map, te_tex_coords + vec2( texel_size.x, 0.0)).r * height_scale * 2.0 - 1.0;
    //float up    = texture(height_map, te_tex_coords + vec2(0.0,  texel_size.y)).r * height_scale * 2.0 - 1.0;
    //float down  = texture(height_map, te_tex_coords + vec2(0.0, -texel_size.y)).r * height_scale * 2.0 - 1.0;
    //te_normal = normalize(vec3(down - up, 2.0, left - right));

    // TBN Matrix
    /*
    mat3 normal_matrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normal_matrix * in_tangent);
    T = normalize(T - dot(T, te_normal) * v_normal);
    vec3 B = cross(v_normal, T);
    te_tbn_matrix = mat3(T, B, v_normal);
    */
}
