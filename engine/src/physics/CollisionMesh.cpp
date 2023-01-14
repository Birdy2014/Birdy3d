#include "physics/CollisionMesh.hpp"

#include <algorithm>
#include <ranges>

namespace Birdy3d::physics {

    CollisionMesh::CollisionMesh(std::vector<glm::vec3> const vertices)
        : m_vertices{vertices}
    {
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

}
