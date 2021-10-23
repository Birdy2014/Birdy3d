#pragma once

#include "ecs/Component.hpp"
#include "events/EventBus.hpp"
#include "physics/ConvexMeshGenerators.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Birdy3d {

    class CollisionPoints;
    class Mesh;
    class Model;

    class Collider : public Component {
    public:
        Collider();
        Collider(const std::string&);
        Collider(GenerationMode);
        void start() override;
        CollisionPoints collides(Collider&);
        void render_wireframe(Shader&);
        void serialize(serializer::Adapter&) override;

    private:
        std::string m_model_name;
        std::shared_ptr<Model> m_model;
        GenerationMode m_generation_mode;
        glm::vec3 m_points[4];
        int m_point_count;

        bool collides(const Mesh& mesh_a, const Mesh& mesh_b, const glm::mat4 transform_a, const glm::mat4 transform_b);
        glm::vec3 support(const Mesh& a, const Mesh& b, const glm::mat4 transform_a, const glm::mat4 transform_b, glm::vec3 direction);
        bool line(glm::vec3& direction);
        bool triangle(glm::vec3& direction);
        bool tetrahedron(glm::vec3& direction);
        bool same_direction(glm::vec3 a, glm::vec3 b);
        void push_front(glm::vec3 point);
        bool next_simplex(glm::vec3& direction);

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(Component, Collider);
    };

}
