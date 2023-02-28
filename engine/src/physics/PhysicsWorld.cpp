#include "physics/PhysicsWorld.hpp"

#include "core/Application.hpp"
#include "ecs/Entity.hpp"
#include "events/CollisionEvent.hpp"
#include "events/EventBus.hpp"
#include "physics/ColliderComponent.hpp"
#include "physics/Collision.hpp"

namespace Birdy3d::physics {

    PhysicsWorld::PhysicsWorld(ecs::Entity* scene)
        : m_scene(scene)
    { }

    PhysicsWorld::~PhysicsWorld() { }

    void for_each_combination_of(std::vector<std::shared_ptr<ColliderComponent>> const& items, auto callback)
    {
        for (auto i = items.cbegin(); i != items.cend(); ++i) {
            for (auto j = i + 1; j != items.cend(); ++j) {
                callback(*i->get(), *j->get());
            }
        }
    }

    void PhysicsWorld::update()
    {
        auto collider_components = m_scene->get_components<ColliderComponent>(false, true);
        for_each_combination_of(collider_components, [&](ColliderComponent const& collider_component_1, ColliderComponent const& collider_component_2) {
            auto collider_1 = collider_component_1.collider();
            auto collider_2 = collider_component_2.collider();

            if (!collider_1 || !collider_2)
                return;

            auto collision_iterator = std::ranges::find_if(m_collisions, [&](Collision const& collision) {
                return collision.contains(collider_component_1) && collision.contains(collider_component_2);
            });

            Collision* collision = {};

            if (collision_iterator != m_collisions.end()) {
                collision = &*collision_iterator;
            } else {
                collision = &m_collisions.emplace_back(collider_component_1, collider_component_2);
            }

            bool collided_last_frame = collision->points.has_value();
            auto optional_points = collider_1->compute_collision(*collider_1, *collider_2, collider_component_1.entity->transform, collider_component_2.entity->transform);
            collision->points = optional_points;
            if (optional_points.has_value()) {
                if (collided_last_frame)
                    core::Application::event_bus->emit<events::CollisionEvent>(collider_1, collider_2, events::CollisionEvent::COLLIDING);
                else
                    core::Application::event_bus->emit<events::CollisionEvent>(collider_1, collider_2, events::CollisionEvent::ENTER);
            } else {
                if (collided_last_frame) {
                    core::Application::event_bus->emit<events::CollisionEvent>(collider_1, collider_2, events::CollisionEvent::EXIT);
                }
            }
        });
    }

}
