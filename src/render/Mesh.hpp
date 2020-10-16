#ifndef BIRDY3D_MESH_HPP
#define BIRDY3D_MESH_HPP

#include <vector>
#include "Vertex.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    // without texture
    glm::vec3 color;
    float specular;
    glm::vec3 emissive;
    bool useTexture;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, glm::vec3 color, float specular, glm::vec3 emissive);
    void draw(Shader *shader);
    void renderDepth();
    void cleanup();

private:
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

#endif