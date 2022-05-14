#pragma once

#include "render/Mesh.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Birdy3d::physics {

    struct Triangle {
        glm::vec3 a, b, c;

        Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c)
            : a(a)
            , b(b)
            , c(c) { }

        glm::vec3 normal() const;

        /**
         * Returns true if the point is outside of the triangle.
         */
        bool outside(const glm::vec3& point) const;

        using Edge = std::pair<glm::vec3, glm::vec3>;

        std::optional<Edge> get_common_edge(Triangle const&) const;
    };

    class IntermediateMesh {
    public:
        IntermediateMesh(glm::vec3, glm::vec3, glm::vec3, glm::vec3);

        std::list<Triangle> const& triangles() const { return m_triangles; }

        void expand(glm::vec3 point);
        std::unique_ptr<render::Mesh> to_mesh() const;

    private:
        std::list<Triangle> m_triangles;
        glm::vec3 m_midpoint;
    };

}
