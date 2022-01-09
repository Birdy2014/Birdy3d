#include "physics/Collider.hpp"

#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Scene.hpp"
#include "events/CollisionEvent.hpp"
#include "physics/Collision.hpp"
#include "render/Mesh.hpp"
#include "render/Model.hpp"
#include "render/ModelComponent.hpp"
#include "render/Vertex.hpp"

namespace Birdy3d::physics {

    Collider::Collider()
        : m_model(nullptr)
        , m_generation_mode(GenerationMode::NONE) { }

    Collider::Collider(const std::string& name)
        : m_model_name(name)
        , m_generation_mode(GenerationMode::NONE) { }

    Collider::Collider(GenerationMode mode)
        : m_model(nullptr)
        , m_generation_mode(mode) { }

    void Collider::start() {
        if (m_generation_mode == GenerationMode::NONE) {
            m_model = core::ResourceManager::get_model(m_model_name).ptr();
        } else {
            auto model_component = entity->get_component<render::ModelComponent>();
            if (!model_component) {
                core::Logger::warn("Entity '{}' doesn't have any ModelComponent", entity->name);
                return;
            }
            auto model = model_component->model();
            if (!model) {
                core::Logger::warn("Entity '{}' doesn't have any model", entity->name);
                return;
            }
            if (m_generation_mode == GenerationMode::COPY)
                m_model = model.ptr();
            else
                m_model = ConvexMeshGenerators::generate_model(m_generation_mode, *model);
        }
    }

    void Collider::render_wireframe(render::Shader& shader) {
        m_model->render_wireframe(*entity, shader);
    }

    void Collider::serialize(serializer::Adapter& adapter) {
        adapter("model_name", m_model_name);
        if (adapter.mode() == serializer::Adapter::Mode::LOAD) {
            int mode;
            adapter("generaton_mode", mode);
            m_generation_mode = (GenerationMode)mode;
        } else {
            int mode = (int)m_generation_mode;
            adapter("generaton_mode", mode);
        }
    }

    CollisionPoints Collider::collides(Collider& collider) {
        CollisionPoints points = { glm::vec3(0), glm::vec3(0), glm::vec3(0), 0, false };
        if (!m_model || !collider.m_model)
            return points;
        for (const auto& own_mesh : m_model->get_meshes()) {
            for (const auto& other_mesh : collider.m_model->get_meshes()) {
                if (collides(*own_mesh.get(), *other_mesh.get(), entity->transform.matrix(), collider.entity->transform.matrix())) {
                    points.hasCollision = true;
                    break;
                }
            }
        }
        return points;
    }

    bool Collider::collides(const render::Mesh& mesh_a, const render::Mesh& mesh_b, const glm::mat4 transform_a, const glm::mat4 transform_b) {
        // FIXME: stop if one of the matrices scales to 0
        m_point_count = 0;
        glm::vec3 s = support(mesh_a, mesh_b, transform_a, transform_b, glm::vec3(1.0f, 0.0f, 0.0f));
        push_front(s);
        glm::vec3 direction = -s;

        while (true) {
            if (direction == glm::vec3(0))
                core::Logger::critical("direction ist 0 in loop. point_count: {}", m_point_count);
            s = support(mesh_a, mesh_b, transform_a, transform_b, direction);

            if (glm::dot(s, direction) <= 0)
                return false;

            if (m_points[0] == s)
                core::Logger::critical("points are the same collides 1 nr: {}", m_point_count);
            push_front(s);

            if (next_simplex(direction))
                return true;
        }
    }

    glm::vec3 Collider::support(const render::Mesh& mesh_a, const render::Mesh& mesh_b, const glm::mat4 transform_a, const glm::mat4 transform_b, glm::vec3 direction) {
        // Transform world direction to local direction
        glm::mat4 inverse_transform_a = glm::inverse(transform_a);
        glm::mat4 inverse_transform_b = glm::inverse(transform_b);
        glm::vec3 local_direction_a = glm::vec3(inverse_transform_a * glm::vec4(direction, 1.0f)) - glm::vec3(inverse_transform_a * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        glm::vec3 local_direction_b = glm::vec3(inverse_transform_b * glm::vec4(direction, 1.0f)) - glm::vec3(inverse_transform_b * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glm::vec3 local_furthest_a = mesh_a.find_furthest_point(local_direction_a);
        glm::vec3 local_furthest_b = mesh_b.find_furthest_point(-local_direction_b);

        // Transform local positions to world positions
        glm::vec3 world_furthest_a = glm::vec3(transform_a * glm::vec4(local_furthest_a, 1.0f));
        glm::vec3 world_furthest_b = glm::vec3(transform_b * glm::vec4(local_furthest_b, 1.0f));

        return world_furthest_a - world_furthest_b;
    }

    void Collider::push_front(glm::vec3 point) {
        if (m_point_count >= 4 || m_point_count < 0)
            core::Logger::critical("Simplex has a maximum size of 4");

        for (int i = m_point_count; i > 0; i--) {
            m_points[i] = m_points[i - 1];
        }

        m_points[0] = point;
        m_point_count++;
    }

    bool Collider::next_simplex(glm::vec3& direction) {
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

    bool Collider::line(glm::vec3& direction) {
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

    bool Collider::triangle(glm::vec3& direction) {
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

    bool Collider::tetrahedron(glm::vec3& direction) {
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

    bool Collider::same_direction(glm::vec3 a, glm::vec3 b) {
        return glm::dot(a, b) > 0;
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, Collider);

}
