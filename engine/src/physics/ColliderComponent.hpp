#pragma once

#include "ecs/Component.hpp"

#include "core/ResourceHandle.hpp"
#include "physics/Collider.hpp"

namespace Birdy3d::physics {

    class ColliderComponent final : public ecs::Component {
    public:
        std::shared_ptr<Collider> collider;

        ColliderComponent();
        ColliderComponent(GenerationMode);

        void start() override;
        void serialize(serializer::Adapter&) override;
        int priority() override { return 10; }
        void render_wireframe(render::Shader&);

    private:
        GenerationMode m_generation_mode = GenerationMode::NONE;
        core::ResourceHandle<Collider> m_collider;

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(ecs::Component, ColliderComponent);
    };

}
