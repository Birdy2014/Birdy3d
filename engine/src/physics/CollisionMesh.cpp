#include "physics/CollisionMesh.hpp"
#include "utils/Ranges.hpp"

#include <algorithm>
#include <ranges>

namespace Birdy3d::physics {

    CollisionMesh::CollisionMesh(Mesh const mesh)
        : m_mesh(mesh)
    {
        m_vertices = utils::to_vector(mesh.vertices | std::views::transform([](render::Vertex vertex) { return vertex.position; }));

        auto last = std::unique(m_vertices.begin(), m_vertices.end());
        m_vertices.erase(last, m_vertices.end());
    }

    glm::vec3 CollisionMesh::find_furthest_point(const glm::vec3 direction) const
    {
        float max = -std::numeric_limits<float>::infinity();
        glm::vec3 furthest_vertex;
        for (auto const& vertex : m_vertices) {
            float dot = glm::dot(vertex, direction);
            if (dot > max) {
                max = dot;
                furthest_vertex = vertex;
            }
        }
        return furthest_vertex;
    }

    [[nodiscard]] render::Mesh const* CollisionMesh::get_render_mesh() const
    {
        if (m_render_mesh.has_value())
            return &m_render_mesh.value();

        m_render_mesh = render::Mesh(m_mesh.vertices, m_mesh.indices);
        return &m_render_mesh.value();
    }

}
