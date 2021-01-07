#ifndef BIRDY3D_MESH_HPP
#define BIRDY3D_MESH_HPP

#include <vector>
#include "render/Vertex.hpp"
#include "render/Texture.hpp"
#include "render/Shader.hpp"

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    // without texture
    glm::vec4 color;
    float specular;
    glm::vec3 emissive;
    bool useTexture;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, glm::vec4 color, float specular, glm::vec3 emissive);
    void render(Shader *shader);
    void renderDepth();
    void cleanup();
    bool hasTransparency();

private:
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

#endif
