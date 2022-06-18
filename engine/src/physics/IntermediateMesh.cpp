#include "physics/IntermediateMesh.hpp"

namespace Birdy3d::physics {

    glm::vec3 Triangle::normal() const {
        glm::vec3 side1 = b - a;
        glm::vec3 side2 = c - a;
        return glm::cross(side1, side2);
    }

    bool Triangle::outside(const glm::vec3& point) const {
        glm::vec3 n = normal();
        float d = glm::dot(n, a);
        return glm::dot(glm::vec4(n, d), glm::vec4(point, -1.0f)) > 0;
    }

    std::optional<Triangle::Edge> Triangle::get_common_edge(Triangle const& other) const {
        auto has_edge = [&other](glm::vec3 a, glm::vec3 b) {
            return (a == other.a || a == other.b || a == other.c) && (b == other.a || b == other.b || b == other.c);
        };
        if (has_edge(a, b))
            return Triangle::Edge { a, b };
        if (has_edge(b, c))
            return Triangle::Edge { b, c };
        if (has_edge(c, a))
            return Triangle::Edge { c, a };
        return {};
    }

    IntermediateMesh::IntermediateMesh(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d) {
        m_midpoint = (a + b + c + d) / glm::vec3(4);
        auto const abc = Triangle { a, b, c };
        if (abc.outside(m_midpoint))
            m_triangles.emplace_back<Triangle>({ a, c, b });
        else
            m_triangles.push_back(abc);

        auto const bcd = Triangle { b, c, d };
        if (bcd.outside(m_midpoint))
            m_triangles.emplace_back<Triangle>({ b, d, c });
        else
            m_triangles.push_back(bcd);

        auto const cda = Triangle { c, d, a };
        if (cda.outside(m_midpoint))
            m_triangles.emplace_back<Triangle>({ c, a, d });
        else
            m_triangles.push_back(cda);

        auto const dab = Triangle { d, a, b };
        if (dab.outside(m_midpoint))
            m_triangles.emplace_back<Triangle>({ d, b, a });
        else
            m_triangles.push_back(dab);
    }

    void IntermediateMesh::expand(glm::vec3 point) {
        // Remove triangles that are facing in the wrong direction
        std::vector<Triangle> removed_triangles;
        for (auto it = m_triangles.begin(); it != m_triangles.end();) {
            if (it->outside(point)) {
                removed_triangles.push_back(*it);
                it = m_triangles.erase(it);
            } else {
                ++it;
            }
        }

        std::list<Triangle> new_triangles;
        for (auto const& removed_triangle : removed_triangles) {
            for (auto const& triangle : m_triangles) {
                auto const optional_edge = removed_triangle.get_common_edge(triangle);
                if (!optional_edge.has_value())
                    continue;

                auto const new_triangle = Triangle { point, optional_edge->first, optional_edge->second };
                if (new_triangle.outside(m_midpoint))
                    new_triangles.emplace_back<Triangle>({ point, optional_edge->second, optional_edge->first });
                else
                    new_triangles.push_back(new_triangle);
            }
        }
        m_triangles.splice(m_triangles.cend(), new_triangles);
    }

    render::Mesh IntermediateMesh::to_mesh() const {
        std::vector<render::Vertex> vertices;
        std::vector<unsigned int> indices;
        std::size_t current_index = 0;
        for (auto const& triangle : m_triangles) {
            indices.push_back(current_index++);
            indices.push_back(current_index++);
            indices.push_back(current_index++);
            auto const normal = triangle.normal();
            vertices.emplace_back<render::Vertex>({ triangle.a, normal, glm::vec3 {}, glm::vec3 {} });
            vertices.emplace_back<render::Vertex>({ triangle.b, normal, glm::vec3 {}, glm::vec3 {} });
            vertices.emplace_back<render::Vertex>({ triangle.c, normal, glm::vec3 {}, glm::vec3 {} });
        }
        return render::Mesh { vertices, indices };
    }

}
