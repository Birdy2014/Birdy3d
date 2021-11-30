#pragma once

#include "ecs/Component.hpp"
#include "events/Forward.hpp"
#include "render/Forward.hpp"
#include "ui/Forward.hpp"
#include <memory>

namespace Birdy3d::utils {

    class FPPlayerController : public ecs::Component {
    public:
        std::weak_ptr<ecs::Entity> flashlight;

        FPPlayerController();
        void start() override;
        void cleanup() override;
        void update() override;
        void serialize(serializer::Adapter&) override;

    private:
        std::weak_ptr<render::Camera> m_cam;
        std::weak_ptr<ui::Widget> m_menu;

        void on_resize(const events::WindowResizeEvent& event);
        void on_key(const events::InputKeyEvent& event);

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(Component, FPPlayerController);
    };

}
