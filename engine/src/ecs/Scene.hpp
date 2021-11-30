#pragma once

#include "core/Base.hpp"
#include "ecs/Entity.hpp"
#include "physics/Forward.hpp"
#include "render/Forward.hpp"

namespace Birdy3d::ecs {

    class Scene : public Entity {
    public:
        std::weak_ptr<render::Camera> main_camera;
        render::Camera* m_current_camera = nullptr;

        Scene(std::string name = "Scene");
        void start() override;
        void update() override;
        void serialize(serializer::Adapter&) override;

    private:
        std::unique_ptr<physics::PhysicsWorld> m_physics_world;

        BIRDY3D_REGISTER_TYPE_DEC(Scene);
    };

}
