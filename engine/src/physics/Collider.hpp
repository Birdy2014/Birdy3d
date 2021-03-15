#pragma once

#include "core/Component.hpp"
#include "events/EventBus.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Birdy3d {

    class CollisionShape;

    class Collider : public Component {
    public:
        Collider();
        Collider(CollisionShape* shape);
        void addShape(CollisionShape* shape);
        bool collides(CollisionShape* shape);
        void start() override;
        void update() override;

    private:
        std::vector<CollisionShape*> shapes;
        glm::vec3 points[4];
        int n_points;

        bool collides(CollisionShape* a, CollisionShape* b);
        glm::vec3 support(CollisionShape* a, CollisionShape* b, glm::vec3 direction);
        bool line(glm::vec3& direction);
        bool triangle(glm::vec3& direction);
        bool tetrahedron(glm::vec3& direction);
        bool sameDirection(glm::vec3 a, glm::vec3 b);
        void push_front(glm::vec3 point);
        bool nextSimplex(glm::vec3& direction);
    };

}
