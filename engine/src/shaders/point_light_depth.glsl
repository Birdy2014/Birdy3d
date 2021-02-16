#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main() {
    gl_Position = model * vec4(aPos, 1.0);
}

#type geometry
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main() {
    for(int face = 0; face < 6; face++) {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int vert = 0; vert < 3; vert++) {
            FragPos = gl_in[vert].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}

#type fragment
#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main() {
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);

    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;

    // write this as modified depth
    gl_FragDepth = lightDistance;
}
