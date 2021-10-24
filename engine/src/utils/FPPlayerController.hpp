#pragma once

#include "ecs/Component.hpp"
#include <memory>

namespace Birdy3d {

    class Camera;
    class InputKeyEvent;
    class Widget;
    class WindowResizeEvent;

    class FPPlayerController : public Component {
    public:
        std::weak_ptr<Entity> flashlight;

        FPPlayerController();
        void start() override;
        void cleanup() override;
        void update() override;
        void serialize(serializer::Adapter&) override;

    private:
        std::weak_ptr<Camera> m_cam;
        std::weak_ptr<Widget> m_menu;

        void on_resize(const WindowResizeEvent& event);
        void on_key(const InputKeyEvent& event);

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(Component, FPPlayerController);
    };

}
