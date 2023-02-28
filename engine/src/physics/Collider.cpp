#include "physics/Collider.hpp"

#include "core/Logger.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Transform.hpp"
#include "physics/Collision.hpp"
#include "physics/CollisionSphere.hpp"
#include "render/Mesh.hpp"
#include "render/Shader.hpp"

namespace Birdy3d::physics {

    Collider::Collider()
        : m_generation_mode(GenerationMode::NONE)
    { }

    Collider::Collider(std::vector<std::unique_ptr<CollisionShape>> shapes)
        : m_collision_shapes(std::move(shapes))
    { }

    void Collider::render_wireframe(ecs::Entity const& entity, render::Shader const& shader) const
    {
        glm::mat4 model = entity.transform.global_matrix();
        shader.use();
        shader.set_mat4("model", model);
        for (auto const& collision_shape : m_collision_shapes) {
            auto render_mesh = collision_shape->get_render_mesh();
            if (render_mesh)
                render_mesh->render_wireframe();
        }
    }

    std::optional<CollisionPoints> Collider::compute_collision(Collider const& collider_a, Collider const& collider_b, ecs::Transform3d const& transform_a, ecs::Transform3d const& transform_b) const
    {
        for (auto const& shape_a : collider_a.m_collision_shapes) {
            for (auto const& shape_b : collider_b.m_collision_shapes) {
                auto sphere_a = dynamic_cast<CollisionSphere*>(shape_a.get());
                auto sphere_b = dynamic_cast<CollisionSphere*>(shape_b.get());
                if (sphere_a && sphere_b) {
                    if (auto points = compute_shape_collision_spheres(*sphere_a, *sphere_b, transform_a, transform_b); points.has_value()) {
                        return points;
                    }
                    continue;
                }

                if (auto points = compute_shape_collision_gjk(*shape_a.get(), *shape_b.get(), transform_a, transform_b); points.has_value()) {
                    return points;
                }
            }
        }

        return {};
    }

    std::optional<CollisionPoints> Collider::compute_shape_collision_gjk(CollisionShape const& shape_a, CollisionShape const& shape_b, ecs::Transform3d const& transform_a, ecs::Transform3d const& transform_b) const
    {
        // FIXME: stop if one of the matrices scales to 0
        m_point_count = 0;
        glm::vec3 s = support(shape_a, shape_b, transform_a, transform_b, glm::vec3(1.0f, 0.0f, 0.0f));
        push_front(s);
        glm::vec3 direction = -s;

        while (true) {
            if (direction == glm::vec3(0))
                core::Logger::critical("direction ist 0 in loop. point_count: {}", m_point_count);
            s = support(shape_a, shape_b, transform_a, transform_b, direction);

            if (glm::dot(s, direction) <= 0)
                return {};

            if (m_points[0] == s)
                core::Logger::critical("points are the same collides 1 nr: {}", m_point_count);
            push_front(s);

            if (next_simplex(direction))
                return CollisionPoints{};
        }
    }

    std::optional<CollisionPoints> Collider::compute_shape_collision_spheres(CollisionSphere const& shape_a, CollisionSphere const& shape_b, ecs::Transform3d const& transform_a, ecs::Transform3d const& transform_b) const
    {
        auto shape_a_center = transform_a.world_position();
        auto shape_b_center = transform_b.world_position();

        auto center_distance = glm::length(shape_b_center - shape_a_center);

        auto direction_a_to_b = shape_b_center - shape_a_center;
        auto direction_b_to_a = -direction_a_to_b;

        auto world_shape_a_furthest = transform_a.local_to_global(shape_a.find_furthest_point(transform_a.global_to_local(direction_a_to_b)));
        auto world_shape_b_furthest = transform_b.local_to_global(shape_b.find_furthest_point(transform_b.global_to_local(direction_b_to_a)));

        auto radius_sum = glm::length(world_shape_a_furthest - shape_a_center) + glm::length(world_shape_b_furthest - shape_b_center);

        if (radius_sum < center_distance)
            return {};

        return CollisionPoints{
            .furthest_a = world_shape_a_furthest,
            .furthest_b = world_shape_b_furthest,
            .normal = glm::normalize(direction_a_to_b),
            .depth = radius_sum - center_distance};
    }

    glm::vec3 Collider::support(CollisionShape const& mesh_a, CollisionShape const& mesh_b, ecs::Transform3d const& transform_a, ecs::Transform3d const& transform_b, glm::vec3 direction) const
    {
        glm::vec3 local_direction_a = transform_a.global_to_local(direction) - transform_a.global_to_local(glm::vec3(0.0f));
        glm::vec3 local_direction_b = transform_b.global_to_local(direction) - transform_b.global_to_local(glm::vec3(0.0f));

        glm::vec3 local_furthest_a = mesh_a.find_furthest_point(local_direction_a);
        glm::vec3 local_furthest_b = mesh_b.find_furthest_point(-local_direction_b);

        glm::vec3 world_furthest_a = transform_a.local_to_global(local_furthest_a);
        glm::vec3 world_furthest_b = transform_b.local_to_global(local_furthest_b);

        return world_furthest_a - world_furthest_b;
    }

    void Collider::push_front(glm::vec3 point) const
    {
        if (m_point_count >= 4 || m_point_count < 0)
            core::Logger::critical("Simplex has a maximum size of 4");

        for (int i = m_point_count; i > 0; i--) {
            m_points[i] = m_points[i - 1];
        }

        m_points[0] = point;
        m_point_count++;
    }

    bool Collider::next_simplex(glm::vec3& direction) const
    {
        switch (m_point_count) {
        case 2:
            return line(direction);
        case 3:
            return triangle(direction);
        case 4:
            return tetrahedron(direction);
        }

        return false;
    }

    bool Collider::line(glm::vec3& direction) const
    {
        glm::vec3 a = m_points[0];
        glm::vec3 b = m_points[1];

        glm::vec3 ab = b - a;
        glm::vec3 ao = -a;

        if (same_direction(ab, ao)) {
            direction = glm::cross(glm::cross(ab, ao), ab);
        } else {
            m_point_count--;
            direction = ao;
        }

        return false;
    }

    bool Collider::triangle(glm::vec3& direction) const
    {
        glm::vec3 a = m_points[0];
        glm::vec3 b = m_points[1];
        glm::vec3 c = m_points[2];

        glm::vec3 ab = b - a;
        glm::vec3 ac = c - a;
        glm::vec3 ao = -a;

        glm::vec3 abc = glm::cross(ab, ac);

        if (same_direction(glm::cross(abc, ac), ao)) {
            if (same_direction(ac, ao)) {
                m_points[1] = c;
                m_point_count--;
                direction = glm::cross(glm::cross(ac, ao), ac);
            } else {
                m_point_count--;
                return line(direction);
            }
        } else {
            if (same_direction(glm::cross(ab, abc), ao)) {
                m_point_count--;
                return line(direction);
            } else {
                // Origin is inside of triangle
                if (same_direction(abc, ao)) {
                    direction = abc;
                } else {
                    m_points[1] = c;
                    m_points[2] = b;
                    direction = -abc;
                }
            }
        }
        return false;
    }

    bool Collider::tetrahedron(glm::vec3& direction) const
    {
        glm::vec3 a = m_points[0];
        glm::vec3 b = m_points[1];
        glm::vec3 c = m_points[2];
        glm::vec3 d = m_points[3];

        glm::vec3 ab = b - a;
        glm::vec3 ac = c - a;
        glm::vec3 ad = d - a;
        glm::vec3 ao = -a;

        glm::vec3 abc = glm::cross(ab, ac);
        glm::vec3 acd = glm::cross(ac, ad);
        glm::vec3 adb = glm::cross(ad, ab);

        if (same_direction(abc, ao)) {
            m_point_count--;
            return triangle(direction);
        }

        if (same_direction(acd, ao)) {
            m_points[1] = c;
            m_points[2] = d;
            m_point_count--;
            return triangle(direction);
        }

        if (same_direction(adb, ao)) {
            m_points[1] = d;
            m_points[2] = b;
            m_point_count--;
            return triangle(direction);
        }

        return true;
    }

    bool Collider::same_direction(glm::vec3 a, glm::vec3 b) const
    {
        return glm::dot(a, b) > 0;
    }

}
