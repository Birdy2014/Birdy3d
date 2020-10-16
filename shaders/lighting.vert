#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
//out vec4 FragPosLightSpace[2];

//uniform mat4 lightSpaceMatrix[2];
	
void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    /*
    for (int i = 0; i < 2; i++) {
        FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(FragPos, 1.0);
    }
    */
}