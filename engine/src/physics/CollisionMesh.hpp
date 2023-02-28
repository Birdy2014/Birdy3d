#pragma once

#include "physics/CollisionShape.hpp"

#include "physics/IntermediateMesh.hpp"
#include "render/Mesh.hpp"
#include <optional>

namespace Birdy3d::physics {

    class CollisionMesh final : public CollisionShape {
    public:
        CollisionMesh(Mesh const);

        [[nodiscard]] virtual glm::vec3 find_furthest_point(const glm::vec3 direction) const override;

        [[nodiscard]] virtual render::Mesh const* get_render_mesh() const override;

    private:
        std::vector<glm::vec3> m_vertices;
        Mesh const m_mesh;
        std::optional<render::Mesh> mutable m_render_mesh;
    };

}
