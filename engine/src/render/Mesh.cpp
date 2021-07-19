#include "render/Mesh.hpp"

#include "core/Logger.hpp"
#include "render/Shader.hpp"
#include "render/Texture.hpp"
#include "render/Vertex.hpp"

namespace Birdy3d {

    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
        : vertices(vertices)
        , indices(indices) {
        setupMesh();
    }

    Mesh::~Mesh() {
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void Mesh::setupMesh() {
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
        shader.setBool("material.diffuse_map_enabled", material.diffuse_map_enabled);
        shader.setVec4("material.diffuse_color", material.diffuse_color);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.diffuse_map->id);
        shader.setInt("material.diffuse_map", 0);

        shader.setBool("material.specular_map_enabled", material.specular_map_enabled);
        shader.setFloat("material.specular_value", material.specular_value);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.specular_map->id);
        shader.setInt("material.specular_map", 1);

        shader.setBool("material.normal_map_enabled", material.normal_map_enabled);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, material.normal_map->id);
        shader.setInt("material.normal_map", 2);

        shader.setBool("material.emissive_map_enabled", material.emissive_map_enabled);
        shader.setVec4("material.emissive_color", material.emissive_color);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, material.emissive_map->id);
        shader.setInt("material.emissive_map", 3);

        // draw mesh
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::renderDepth() const {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    glm::vec3 Mesh::findFurthestPoint(const glm::vec3 direction) const {
        float max = std::numeric_limits<float>::min();
        glm::vec3 furthestVertex;
        for (Vertex vertex : vertices) {
            float dot = glm::dot(vertex.position, direction);
            if (dot > max) {
                max = dot;
                furthestVertex = vertex.position;
            }
        }
        return furthestVertex;
    }

}
