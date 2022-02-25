#type vertex
layout (location = 0) in vec3 in_pos;

uniform mat4 model;

void main() {
    gl_Position = model * vec4(in_pos, 1.0);
}

#type geometry
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadow_matrices[6];

out vec4 frag_pos; // FragPos from GS (output per emitvertex)

void main() {
    for(int face = 0; face < 6; face++) {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int vert = 0; vert < 3; vert++) {
            frag_pos = gl_in[vert].gl_Position;
            gl_Position = shadow_matrices[face] * frag_pos;
            EmitVertex();
        }
        EndPrimitive();
    }
}

#type fragment
in vec4 frag_pos;

uniform vec3 light_pos;
uniform float far_plane;

void main() {
    // get distance between fragment and light source
    float light_distance = length(frag_pos.xyz - light_pos);

    // map to [0;1] range by dividing by far_plane
    light_distance = light_distance / far_plane;

    // write this as modified depth
    gl_FragDepth = light_distance;
}
