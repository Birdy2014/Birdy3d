#pragma once

#include "core/Base.hpp"
#include "render/Forward.hpp"
#include "render/Material.hpp"
#include "render/Texture.hpp"
#include "render/Vertex.hpp"

namespace Birdy3d::render {

    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
        ~Mesh();
        void render(const Shader& shader, const Material& material) const;
        void render_depth(bool use_tesselation) const;
        void render_wireframe(bool use_tesselation) const;
        glm::vec3 find_furthest_point(const glm::vec3 direction) const;

    private:
        unsigned int m_vao, m_vbo, m_ebo;

        void setup();
    };

}
