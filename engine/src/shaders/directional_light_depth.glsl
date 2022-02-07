#parameter SHADOW_CASCADE_SIZE 1

#type vertex
layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main() {
    gl_Position = model * vec4(aPos, 1.0);
}

#type geometry
layout (triangles, invocations = SHADOW_CASCADE_SIZE) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 light_space_matrices[SHADOW_CASCADE_SIZE];

void main() {
    for (int i = 0; i < 3; ++i) {
        gl_Position = light_space_matrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}

#type fragment
out vec4 FragColor;

void main() {

}
