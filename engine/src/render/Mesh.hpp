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

        // Class is non-copyable but movable
        Mesh(Mesh const&) = delete;
        Mesh& operator=(Mesh const&) = delete;

        Mesh(Mesh&& other)
            : vertices(other.vertices)
            , indices(other.indices)
            , m_vao(other.m_vao)
            , m_vbo(other.m_vao)
            , m_ebo(other.m_ebo) {
            other.m_vao = 0;
            other.m_vbo = 0;
            other.m_ebo = 0;
        }

        Mesh& operator=(Mesh&& other) {
            if (this != &other) {
                release();
            }
            return *this;
        }

        void render(const Shader& shader, const Material& material) const;
        void render_depth() const;
        void render_wireframe() const;
        glm::vec3 find_furthest_point(const glm::vec3 direction) const;

    private:
        unsigned int m_vao, m_vbo, m_ebo;

        void setup();
        void release();
    };

}
