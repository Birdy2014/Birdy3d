#pragma once

#include "ecs/Forward.hpp"
#include "physics/Collision.hpp"
#include "physics/CollisionShape.hpp"
#include "physics/CollisionSphere.hpp"
#include "physics/ConvexMeshGenerators.hpp"
#include "render/Forward.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Birdy3d::physics {

    class Collider {
    public:
        Collider();
        Collider(std::shared_ptr<render::Model>, std::vector<std::unique_ptr<CollisionShape>>);
        std::optional<CollisionPoints> compute_collision(Collider const& collider_a, Collider const& collider_b, ecs::Transform3d const&, ecs::Transform3d const&);
        void render_wireframe(ecs::Entity&, render::Shader&);

    private:
        std::string m_model_name;
        std::shared_ptr<render::Model> m_render_model;
        std::vector<std::unique_ptr<CollisionShape>> m_collision_shapes;
        GenerationMode m_generation_mode;
        glm::vec3 m_points[4];
        int m_point_count;

        std::optional<CollisionPoints> compute_shape_collision_gjk(CollisionShape const& shape_a, CollisionShape const& shape_b, ecs::Transform3d const&, ecs::Transform3d const&);
        std::optional<CollisionPoints> compute_shape_collision_spheres(CollisionSphere const& shape_a, CollisionSphere const& shape_b, ecs::Transform3d const&, ecs::Transform3d const&);
        glm::vec3 support(CollisionShape const& a, CollisionShape const& b, ecs::Transform3d const& transform_a, ecs::Transform3d const& transform_b, glm::vec3 direction);
        bool line(glm::vec3& direction);
        bool triangle(glm::vec3& direction);
        bool tetrahedron(glm::vec3& direction);
        bool same_direction(glm::vec3 a, glm::vec3 b);
        void push_front(glm::vec3 point);
        bool next_simplex(glm::vec3& direction);
    };

}
