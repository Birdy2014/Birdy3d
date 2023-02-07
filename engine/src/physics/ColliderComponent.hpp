#pragma once

#include "ecs/Component.hpp"

#include "core/ResourceHandle.hpp"
#include "physics/Collider.hpp"

namespace Birdy3d::physics {

    class ColliderComponent final : public ecs::Component {
    public:
        ColliderComponent();
        ColliderComponent(GenerationMode);

        void start() override;
        void cleanup() override;
        void serialize(serializer::Adapter&) override;
        int priority() override { return 10; }
        void render_wireframe(render::Shader&);

        [[nodiscard]] std::shared_ptr<Collider> collider() const { return m_collider.ptr(); }

    private:
        GenerationMode m_generation_mode = GenerationMode::NONE;
        core::ResourceHandle<Collider> m_collider;

        void on_resource_loaded(events::ResourceLoadEvent const& event);
        void reload_collider();

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(ecs::Component, ColliderComponent);
    };

}
