#include "physics/CollisionSphere.hpp"

#include "render/Model.hpp"
#include "utils/PrimitiveGenerator.hpp"

namespace Birdy3d::physics {

    CollisionSphere::CollisionSphere(float radius)
        : m_radius{radius}
    { }

    glm::vec3 CollisionSphere::find_furthest_point(const glm::vec3 direction) const
    {
        return m_radius * glm::normalize(direction);
    }

    [[nodiscard]] render::Mesh const* CollisionSphere::get_render_mesh() const
    {
        // FIXME: Use a billboard to represent a CollisionSphere?
        if (m_render_model)
            return &m_render_model->get_meshes()[0];

        m_render_model = utils::PrimitiveGenerator::generate_uv_sphere(10);
        return &m_render_model->get_meshes()[0];
    }

}
