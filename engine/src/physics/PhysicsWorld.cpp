#include "physics/PhysicsWorld.hpp"

#include "core/Application.hpp"
#include "core/GameObject.hpp"
#include "events/CollisionEvent.hpp"
#include "physics/Collider.hpp"
#include "physics/Collision.hpp"

namespace Birdy3d {

    PhysicsWorld::PhysicsWorld(GameObject* scene)
        : scene(scene) { }

    void PhysicsWorld::update() {
        std::vector<Collider*> colliders = scene->getComponents<Collider>(false, true);
        for (std::vector<Collider*>::reverse_iterator it = colliders.rbegin(); it != colliders.rend(); it++) {
            Collider* c1 = *it;
            for (Collider* c2 : colliders) {
                if (c1 == c2)
                    continue;
                Collision* collision = nullptr;
                for (Collision* c : collisions) {
                    if (c->contains(c1) && c->contains(c2))
                        collision = c;
                }
                if (!collision) {
                    collision = new Collision(c1, c2);
                    collisions.push_back(collision);
                }
                bool collidedLastFrame = collision->points.hasCollision;
                CollisionPoints points = c1->collides(c2);
                collision->points = points;
                if (points.hasCollision) {
                    if (collidedLastFrame)
                        Application::eventBus->emit<CollisionEvent>(c1, c2, CollisionEvent::COLLIDING);
                    else
                        Application::eventBus->emit<CollisionEvent>(c1, c2, CollisionEvent::ENTER);
                } else {
                    if (collidedLastFrame) {
                        Application::eventBus->emit<CollisionEvent>(c1, c2, CollisionEvent::EXIT);
                    }
                }
            }
            colliders.pop_back();
        }
    }

}
