#pragma once

#include "core/Base.hpp"
#include "physics/PhysicsWorld.hpp"
#include "scene/Entity.hpp"

namespace Birdy3d {

    class Camera;

    class Scene : public Entity {
    public:
        std::weak_ptr<Camera> main_camera;
        Camera* m_current_camera = nullptr;

        Scene(std::string name = "Scene")
            : Entity(name) { }

        void start() override {
            set_scene(this);
            m_physics_world = std::make_unique<PhysicsWorld>(this);
            Entity::start();
        }

        void update() override {
            Entity::update();
            transform.update();
            m_physics_world->update();
            transform.update();
        }

        template <class Archive>
        void serialize(Archive& ar) {
            ar(cereal::base_class<Entity>(this));
            ar(cereal::make_nvp("main_camera", main_camera));
        }

    private:
        std::unique_ptr<PhysicsWorld> m_physics_world;
    };

}
