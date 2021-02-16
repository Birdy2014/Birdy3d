#pragma once

#include "render/Texture.hpp"
#include "render/Vertex.hpp"
#include <vector>

namespace Birdy3d {

    class Shader;

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

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, float specular = 1.0f, glm::vec3 emissive = glm::vec3(0.0f));
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, glm::vec4 color, float specular, glm::vec3 emissive);
        void render(Shader* shader);
        void renderDepth();
        void cleanup();
        bool hasTransparency();

    private:
        unsigned int VAO, VBO, EBO;

        void setupMesh();
    };

}
