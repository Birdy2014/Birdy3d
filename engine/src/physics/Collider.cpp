#include "physics/Collider.hpp"

#include "core/Application.hpp"
#include "core/GameObject.hpp"
#include "core/Logger.hpp"
#include "events/CollisionEvent.hpp"
#include "physics/Collision.hpp"
#include "render/Mesh.hpp"
#include "render/Model.hpp"
#include "render/ModelComponent.hpp"
#include "render/Vertex.hpp"
#include "scene/Scene.hpp"

namespace Birdy3d {

    Collider::Collider(Model* model)
        : m_model(model) { }

    Collider::Collider(GenerationMode mode)
        : m_model(nullptr)
        , m_generation_mode(mode) { }

    void Collider::start() {
        if (!m_model) {
            Model* model = object->getComponent<ModelComponent>()->model.get();
            if (!model) {
                Logger::warn("GameObject doesn't have any model");
                return;
            }
            m_model = ConvexMeshGenerators::generate_model(m_generation_mode, model);
        }
    }

    CollisionPoints Collider::collides(Collider* collider) {
        CollisionPoints points = { glm::vec3(0), glm::vec3(0), glm::vec3(0), 0, false };
        for (Mesh* own_mesh : m_model->getMeshes()) {
            for (Mesh* other_mesh : collider->m_model->getMeshes()) {
                if (collides(own_mesh, other_mesh, object->transform.matrix(), collider->object->transform.matrix())) {
                    points.hasCollision = true;
                    break;
                }
            }
        }
        return points;
    }

    bool Collider::collides(Mesh* mesh_a, Mesh* mesh_b, glm::mat4 transform_a, glm::mat4 transform_b) {
        m_point_count = 0;
        glm::vec3 s = support(mesh_a, mesh_b, transform_a, transform_b, glm::vec3(1.0f, 0.0f, 0.0f));
        push_front(s);
        glm::vec3 direction = -s;

        while (true) {
            if (direction == glm::vec3(0))
                Logger::error("direction ist 0 in loop. point_count: ", m_point_count);
            s = support(mesh_a, mesh_b, transform_a, transform_b, direction);

            if (glm::dot(s, direction) <= 0)
                return false;

            if (m_points[0] == s)
                Logger::error("points are the same collides 1 nr:", m_point_count);
            push_front(s);

            if (nextSimplex(direction))
                return true;
        }
    }

    glm::vec3 Collider::support(Mesh* mesh_a, Mesh* mesh_b, glm::mat4 transform_a, glm::mat4 transform_b, glm::vec3 direction) {
        // Transform world direction to local direction
        glm::mat4 inverse_transform_a = glm::inverse(transform_a);
        glm::mat4 inverse_transform_b = glm::inverse(transform_b);
        glm::vec3 local_direction_a = glm::vec3(inverse_transform_a * glm::vec4(direction, 1.0f)) - glm::vec3(inverse_transform_a * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        glm::vec3 local_direction_b = glm::vec3(inverse_transform_b * glm::vec4(direction, 1.0f)) - glm::vec3(inverse_transform_b * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glm::vec3 local_furthest_a = mesh_a->findFurthestPoint(local_direction_a);
        glm::vec3 local_furthest_b = mesh_b->findFurthestPoint(-local_direction_b);

        // Transform local positions to world positions
        glm::vec3 world_furthest_a = glm::vec3(transform_a * glm::vec4(local_furthest_a, 1.0f));
        glm::vec3 world_furthest_b = glm::vec3(transform_b * glm::vec4(local_furthest_b, 1.0f));

        return world_furthest_a - world_furthest_b;
    }

    void Collider::push_front(glm::vec3 point) {
        if (m_point_count >= 4 || m_point_count < 0)
            Logger::error("Simplex has a maximum size of 4");

        for (int i = m_point_count; i > 0; i--) {
            m_points[i] = m_points[i - 1];
        }

        m_points[0] = point;
        m_point_count++;
    }

    bool Collider::nextSimplex(glm::vec3& direction) {
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

        if (sameDirection(ab, ao)) {
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

        if (sameDirection(glm::cross(abc, ac), ao)) {
            if (sameDirection(ac, ao)) {
                m_points[1] = c;
                m_point_count--;
                direction = glm::cross(glm::cross(ac, ao), ac);
            } else {
                m_point_count--;
                return line(direction);
            }
        } else {
            if (sameDirection(glm::cross(ab, abc), ao)) {
                m_point_count--;
                return line(direction);
            } else {
                // Origin is inside of triangle
                if (sameDirection(abc, ao)) {
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

        if (sameDirection(abc, ao)) {
            m_point_count--;
            return triangle(direction);
        }

        if (sameDirection(acd, ao)) {
            m_points[1] = c;
            m_points[2] = d;
            m_point_count--;
            return triangle(direction);
        }

        if (sameDirection(adb, ao)) {
            m_points[1] = d;
            m_points[2] = b;
            m_point_count--;
            return triangle(direction);
        }

        return true;
    }

    bool Collider::sameDirection(glm::vec3 a, glm::vec3 b) {
        return glm::dot(a, b) > 0;
    }

}
