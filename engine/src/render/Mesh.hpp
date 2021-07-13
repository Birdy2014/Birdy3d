#pragma once

#include "render/Material.hpp"
#include "render/Texture.hpp"
#include "render/Vertex.hpp"
#include <vector>

namespace Birdy3d {

    class Shader;

    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
        ~Mesh();
        void render(Shader* shader, const Material& material);
        void renderDepth();
        glm::vec3 findFurthestPoint(glm::vec3 direction);

    private:
        unsigned int m_vao, m_vbo, m_ebo;

        void setupMesh();
    };

}
