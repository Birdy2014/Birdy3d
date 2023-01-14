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
            for (auto j = i; j != items.cend(); ++j) {
                if (i == j)
                    continue;
                callback(*i->get(), *j->get());
            }
        }
    }

    void PhysicsWorld::update()
    {
        auto collider_components = m_scene->get_components<ColliderComponent>(false, true);
        for_each_combination_of(collider_components, [&](ColliderComponent const& collider_component_1, ColliderComponent const& collider_component_2) {
            auto collider_1 = collider_component_1.collider;
            auto collider_2 = collider_component_2.collider;

            if (!collider_1 || !collider_2)
                return;

            auto collision_iterator = std::ranges::find_if(m_collisions, [&](std::unique_ptr<Collision> const& collision) {
                return collision->contains(collider_1.get()) && collision->contains(collider_2.get());
            });

            Collision* collision = {};

            if (collision_iterator != m_collisions.end()) {
                collision = collision_iterator->get();
            } else {
                auto unique_collision = std::make_unique<Collision>(collider_1.get(), collider_2.get());
                collision = unique_collision.get();
                m_collisions.push_back(std::move(unique_collision));
            }

            bool collided_last_frame = collision->points.has_collision;
            CollisionPoints points = collider_1->compute_collision(*collider_1.get(), *collider_2.get(), collider_component_1.entity->transform.global_matrix(), collider_component_2.entity->transform.global_matrix());
            collision->points = points;
            if (points.has_collision) {
                if (collided_last_frame)
                    core::Application::event_bus->emit<events::CollisionEvent>(collider_component_1.collider, collider_component_2.collider, events::CollisionEvent::COLLIDING);
                else
                    core::Application::event_bus->emit<events::CollisionEvent>(collider_component_1.collider, collider_component_2.collider, events::CollisionEvent::ENTER);
            } else {
                if (collided_last_frame) {
                    core::Application::event_bus->emit<events::CollisionEvent>(collider_component_1.collider, collider_component_2.collider, events::CollisionEvent::EXIT);
                }
            }
        });
    }

}
