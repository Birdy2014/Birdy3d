#include "ecs/Scene.hpp"

#include "physics/PhysicsWorld.hpp"
#include "render/Camera.hpp"

namespace Birdy3d::ecs {

    Scene::Scene(std::string name)
        : Entity(name) { }

    void Scene::start() {
        set_scene(this);
        m_physics_world = std::make_unique<physics::PhysicsWorld>(this);
        transform.update(true);
        Entity::start();
    }

    void Scene::update() {
        Entity::update();
        transform.update();
        m_physics_world->update();
        transform.update();
    }

    void Scene::serialize(serializer::Adapter& adapter) {
        Entity::serialize(adapter);
        adapter("main_camera", main_camera);
    }

    BIRDY3D_REGISTER_TYPE_DEF(Scene);

}
