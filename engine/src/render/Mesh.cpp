#include "render/Mesh.hpp"

#include "core/Logger.hpp"
#include "render/Shader.hpp"
#include "render/Texture.hpp"
#include "render/Vertex.hpp"

namespace Birdy3d::render {

    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
        : vertices(vertices)
        , indices(indices) {
        setup();
    }

    Mesh::~Mesh() {
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void Mesh::setup() {
        // generate vao, vbo and ebo
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        glBindVertexArray(m_vao);

        // load vertices into vbo
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        // load indices into ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // inform OpenGL how to interpret the vertex data inside the vbo
        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        // vertex tangent coords
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

        glBindVertexArray(0);
    }

    void Mesh::render(const Shader& shader, const Material& material) const {
        material.use(shader);

        // draw mesh
        glBindVertexArray(m_vao);
        if (shader.has_tesselation())
            glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::render_depth(bool use_tesselation) const {
        glBindVertexArray(m_vao);
        if (use_tesselation)
            glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::render_wireframe(bool use_tesselation) const {
        glBindVertexArray(m_vao);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (use_tesselation)
            glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glm::vec3 Mesh::find_furthest_point(const glm::vec3 direction) const {
        float max = -std::numeric_limits<float>::infinity();
        glm::vec3 furthest_vertex;
        for (Vertex vertex : vertices) {
            float dot = glm::dot(vertex.position, direction);
            if (dot > max) {
                max = dot;
                furthest_vertex = vertex.position;
            }
        }
        return furthest_vertex;
    }

}
