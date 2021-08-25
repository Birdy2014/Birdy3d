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

        Scene(std::string name = "Scene")
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

        template <class Archive>
        void serialize(Archive& ar) {
            ar(cereal::base_class<GameObject>(this));
            ar(cereal::make_nvp("main_camera", main_camera));
        }

    private:
        std::unique_ptr<PhysicsWorld> m_physics_world;
    };

}
