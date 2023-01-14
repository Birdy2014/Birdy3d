#pragma once

#include "ecs/Forward.hpp"
#include "physics/CollisionShape.hpp"
#include "physics/ConvexMeshGenerators.hpp"
#include "physics/Forward.hpp"
#include "render/Forward.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Birdy3d::physics {

    class Collider {
    public:
        Collider();
        Collider(std::shared_ptr<render::Model>, std::vector<std::unique_ptr<CollisionShape>>);
        CollisionPoints compute_collision(Collider const& collider_a, Collider const& collider_b, glm::mat4 const transform_a, glm::mat4 const transform_b);
        void render_wireframe(ecs::Entity&, render::Shader&);

    private:
        std::string m_model_name;
        std::shared_ptr<render::Model> m_render_model;
        std::vector<std::unique_ptr<CollisionShape>> m_collision_shapes;
        GenerationMode m_generation_mode;
        glm::vec3 m_points[4];
        int m_point_count;

        bool collides(CollisionShape const& shape_a, CollisionShape const& shape_b, glm::mat4 const transform_a, glm::mat4 const transform_b);
        glm::vec3 support(CollisionShape const& a, CollisionShape const& b, glm::mat4 const transform_a, glm::mat4 const transform_b, glm::vec3 direction);
        bool line(glm::vec3& direction);
        bool triangle(glm::vec3& direction);
        bool tetrahedron(glm::vec3& direction);
        bool same_direction(glm::vec3 a, glm::vec3 b);
        void push_front(glm::vec3 point);
        bool next_simplex(glm::vec3& direction);
    };

}
