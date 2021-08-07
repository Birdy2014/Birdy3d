#include "physics/PhysicsWorld.hpp"

#include "core/Application.hpp"
#include "events/CollisionEvent.hpp"
#include "physics/Collider.hpp"
#include "physics/Collision.hpp"
#include "scene/GameObject.hpp"

namespace Birdy3d {

    PhysicsWorld::PhysicsWorld(GameObject* scene)
        : m_scene(scene) { }

    PhysicsWorld::~PhysicsWorld() { }

    void PhysicsWorld::update() {
        auto colliders = m_scene->get_components<Collider>(false, true);
        for (auto it = colliders.rbegin(); it != colliders.rend(); it++) {
            auto c1 = *it;
            for (auto c2 : colliders) {
                if (c1 == c2)
                    continue;
                Collision* collision = nullptr;
                for (const auto& c : m_collisions) {
                    if (c->contains(c1.get()) && c->contains(c2.get())) {
                        collision = c.get();
                        break;
                    }
                }
                if (!collision) {
                    auto unique_collision = std::make_unique<Collision>(c1.get(), c2.get());
                    collision = unique_collision.get();
                    m_collisions.push_back(std::move(unique_collision));
                }
                bool collidedLastFrame = collision->points.hasCollision;
                CollisionPoints points = c1->collides(*c2.get());
                collision->points = points;
                if (points.hasCollision) {
                    if (collidedLastFrame)
                        Application::event_bus->emit<CollisionEvent>(c1, c2, CollisionEvent::COLLIDING);
                    else
                        Application::event_bus->emit<CollisionEvent>(c1, c2, CollisionEvent::ENTER);
                } else {
                    if (collidedLastFrame) {
                        Application::event_bus->emit<CollisionEvent>(c1, c2, CollisionEvent::EXIT);
                    }
                }
            }
            colliders.pop_back();
        }
    }

}
