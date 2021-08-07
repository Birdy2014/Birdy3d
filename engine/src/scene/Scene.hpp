#pragma once

#include "core/Base.hpp"
#include "physics/PhysicsWorld.hpp"
#include "scene/GameObject.hpp"

namespace Birdy3d {

    class Camera;

    class Scene : public GameObject {
    public:
        std::weak_ptr<Camera> main_camera;
        Camera* m_current_camera = nullptr;

        Scene(std::string name)
            : GameObject(name) { }

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
