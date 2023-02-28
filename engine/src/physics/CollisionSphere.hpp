#pragma once

#include "physics/CollisionShape.hpp"

#include "render/Model.hpp"

namespace Birdy3d::physics {

    class CollisionSphere final : public CollisionShape {
    public:
        CollisionSphere(float radius);

        [[nodiscard]] virtual glm::vec3 find_furthest_point(const glm::vec3 direction) const override;

        [[nodiscard]] virtual render::Mesh const* get_render_mesh() const override;

    private:
        float m_radius;
        std::unique_ptr<render::Model> mutable m_render_model;
    };

}
