#include "physics/Collider.hpp"

#include "core/Application.hpp"
#include "core/GameObject.hpp"
#include "core/Logger.hpp"
#include "events/CollisionEvent.hpp"
#include "physics/CollisionMesh.hpp"
#include "physics/CollisionShape.hpp"
#include "render/Mesh.hpp"
#include "render/Model.hpp"
#include "render/Vertex.hpp"

namespace Birdy3d {

    Collider::Collider() { }

    Collider::Collider(CollisionShape* shape) {
        addShape(shape);
    }

    void Collider::addShape(CollisionShape* shape) {
        shape->collider = this;
        this->shapes.push_back(shape);
    }

    bool Collider::collides(CollisionShape* shape) {
        for (CollisionShape* s : this->shapes) {
            if (collides(s, shape))
                return true;
        }
        return false;
    }

    void Collider::start() {
        if (shapes.empty()) {
            Model* model = object->getComponent<Model>();
            for (const Mesh& mesh : model->getMeshes()) {
                // TODO: approximate convex decomposition
                std::vector<glm::vec3> collisionMesh;
                for (const Vertex& currentVertex : mesh.vertices) {
                    bool exists = false;
                    for (const glm::vec3& v : collisionMesh) {
                        if (currentVertex.position == v) {
                            exists = true;
                            break;
                        }
                    }
                    if (!exists)
                        collisionMesh.push_back(currentVertex.position);
                }
                Logger::debug("generated collision mesh size: " + std::to_string(collisionMesh.size()));
                addShape(new CollisionMesh(collisionMesh));
            }
        }
    }

    void Collider::update() {
        Collider* otherCollider = nullptr;
        for (Collider* c : this->object->scene->getComponents<Collider>(false, true)) {
            if (c == this)
                continue;
            if (otherCollider)
                break;
            for (CollisionShape* s : this->shapes) {
                if (c->collides(s)) {
                    otherCollider = c;
                    break;
                }
            }
        }
        if (otherCollider)
            Application::eventBus->emit(new CollisionEvent(this, otherCollider));
    }

    bool Collider::collides(CollisionShape* a, CollisionShape* b) {
        n_points = 0;
        glm::vec3 s = support(a, b, glm::vec3(1.0f, 0.0f, 0.0f));
        push_front(s);
        glm::vec3 direction = -s;

        while (true) {
            if (direction == glm::vec3(0))
                Logger::error("direction ist 0 in loop. n_points: " + std::to_string(n_points));
            s = support(a, b, direction);

            if (glm::dot(s, direction) <= 0)
                return false;

            if (points[0] == s)
                Logger::error("points are the same collides 1 nr:" + std::to_string(n_points));
            push_front(s);

            if (nextSimplex(direction))
                return true;
        }
    }

    glm::vec3 Collider::support(CollisionShape* a, CollisionShape* b, glm::vec3 direction) {
        glm::vec3 furthestA = a->findFurthestPoint(direction);
        glm::vec3 furthestB = b->findFurthestPoint(-direction);
        return furthestA - furthestB;
    }

    void Collider::push_front(glm::vec3 point) {
        if (n_points >= 4 || n_points < 0)
            Logger::error("Simplex has a maximum size of 4");

        for (int i = this->n_points; i > 0; i--) {
            points[i] = points[i - 1];
        }

        points[0] = point;
        n_points++;
    }

    bool Collider::nextSimplex(glm::vec3& direction) {
        switch (n_points) {
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
        glm::vec3 a = points[0];
        glm::vec3 b = points[1];

        glm::vec3 ab = b - a;
        glm::vec3 ao = -a;

        if (sameDirection(ab, ao)) {
            direction = glm::cross(glm::cross(ab, ao), ab);
        } else {
            n_points--;
            direction = ao;
        }

        return false;
    }

    bool Collider::triangle(glm::vec3& direction) {
        glm::vec3 a = points[0];
        glm::vec3 b = points[1];
        glm::vec3 c = points[2];

        glm::vec3 ab = b - a;
        glm::vec3 ac = c - a;
        glm::vec3 ao = -a;

        glm::vec3 abc = glm::cross(ab, ac);

        if (sameDirection(glm::cross(abc, ac), ao)) {
            if (sameDirection(ac, ao)) {
                points[1] = c;
                n_points--;
                direction = glm::cross(glm::cross(ac, ao), ac);
            } else {
                n_points--;
                return line(direction);
            }
        } else {
            if (sameDirection(glm::cross(ab, abc), ao)) {
                n_points--;
                return line(direction);
            } else {
                // Origin is inside of triangle
                if (sameDirection(abc, ao)) {
                    direction = abc;
                } else {
                    points[1] = c;
                    points[2] = b;
                    direction = -abc;
                }
            }
        }
        return false;
    }

    bool Collider::tetrahedron(glm::vec3& direction) {
        glm::vec3 a = points[0];
        glm::vec3 b = points[1];
        glm::vec3 c = points[2];
        glm::vec3 d = points[3];

        glm::vec3 ab = b - a;
        glm::vec3 ac = c - a;
        glm::vec3 ad = d - a;
        glm::vec3 ao = -a;

        glm::vec3 abc = glm::cross(ab, ac);
        glm::vec3 acd = glm::cross(ac, ad);
        glm::vec3 adb = glm::cross(ad, ab);

        if (sameDirection(abc, ao)) {
            n_points--;
            return triangle(direction);
        }

        if (sameDirection(acd, ao)) {
            points[1] = c;
            points[2] = d;
            n_points--;
            return triangle(direction);
        }

        if (sameDirection(adb, ao)) {
            points[1] = d;
            points[2] = b;
            n_points--;
            return triangle(direction);
        }

        return true;
    }

    bool Collider::sameDirection(glm::vec3 a, glm::vec3 b) {
        return glm::dot(a, b) > 0;
    }

}
