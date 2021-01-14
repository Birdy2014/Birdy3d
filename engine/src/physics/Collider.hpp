#ifndef BIRDY3D_COLLIDER_HPP
#define BIRDY3D_COLLIDER_HPP

#include "core/Component.hpp"
#include "core/EventDispatcher.hpp"
#include <glm/glm.hpp>
#include <vector>

class CollisionShape;

class Collider : public Component {
public:
    struct EventArg {
        Collider *other;
    };

    enum EventType {
        COLLISION
    };

    EventDispatcher<EventArg> *eventDispatcher = new EventDispatcher<EventArg>();

    void addShape(CollisionShape *shape);
    bool collides(CollisionShape *shape);
    void update(float deltaTime) override;

private:
    std::vector<CollisionShape *> shapes;
    glm::vec3 points[4];
    int n_points;

    bool collides(CollisionShape *a, CollisionShape *b);
    glm::vec3 support(CollisionShape *a, CollisionShape *b, glm::vec3 direction);
    bool line(glm::vec3 &direction);
    bool triangle(glm::vec3 &direction);
    bool tetrahedron(glm::vec3 &direction);
    bool sameDirection(glm::vec3 a, glm::vec3 b);
    void push_front(glm::vec3 point);
    bool nextSimplex(glm::vec3 &direction);
};

#endif
