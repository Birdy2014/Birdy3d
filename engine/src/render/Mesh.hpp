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
        void render(const Shader& shader, const Material& material) const;
        void renderDepth() const;
        glm::vec3 findFurthestPoint(const glm::vec3 direction) const;

    private:
        unsigned int m_vao, m_vbo, m_ebo;

        void setupMesh();
    };

}
