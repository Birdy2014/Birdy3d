#pragma once

#include "render/ModelOptions.hpp"
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

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
        void render(Shader* shader, const ModelOptions& options);
        void renderDepth();
        void cleanup();
        bool hasTransparency(const ModelOptions& options);

    private:
        unsigned int VAO, VBO, EBO;

        void setupMesh();
    };

}
