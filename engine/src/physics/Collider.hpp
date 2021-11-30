#pragma once

#include "ecs/Component.hpp"
#include "physics/ConvexMeshGenerators.hpp"
#include "physics/Forward.hpp"
#include "render/Forward.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Birdy3d::physics {

    class Collider : public ecs::Component {
    public:
        Collider();
        Collider(const std::string&);
        Collider(GenerationMode);
        void start() override;
        CollisionPoints collides(Collider&);
        void render_wireframe(render::Shader&);
        void serialize(serializer::Adapter&) override;

    private:
        std::string m_model_name;
        std::shared_ptr<render::Model> m_model;
        GenerationMode m_generation_mode;
        glm::vec3 m_points[4];
        int m_point_count;

        bool collides(const render::Mesh& mesh_a, const render::Mesh& mesh_b, const glm::mat4 transform_a, const glm::mat4 transform_b);
        glm::vec3 support(const render::Mesh& a, const render::Mesh& b, const glm::mat4 transform_a, const glm::mat4 transform_b, glm::vec3 direction);
        bool line(glm::vec3& direction);
        bool triangle(glm::vec3& direction);
        bool tetrahedron(glm::vec3& direction);
        bool same_direction(glm::vec3 a, glm::vec3 b);
        void push_front(glm::vec3 point);
        bool next_simplex(glm::vec3& direction);

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(Component, Collider);
    };

}
