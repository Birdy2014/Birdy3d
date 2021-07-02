#pragma once

#include "core/GameObject.hpp"
#include "physics/PhysicsWorld.hpp"
#include <memory>

namespace Birdy3d {

    class Camera;

    class Scene : public GameObject {
    public:
        Camera* m_current_camera;

        void start() override {
            setScene(this);
            m_physics_world = std::make_unique<PhysicsWorld>(this);
            GameObject::start();
        }

        void update() override {
            GameObject::update();
            m_physics_world->update();
        }

    private:
        std::unique_ptr<PhysicsWorld> m_physics_world;
    };

}
