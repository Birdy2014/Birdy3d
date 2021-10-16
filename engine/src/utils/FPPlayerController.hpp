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

        template <class Archive>
        void serialize(Archive& ar) {
            ar(CEREAL_NVP(flashlight));
        }

    private:
        std::weak_ptr<Camera> m_cam;
        std::weak_ptr<Widget> m_menu;

        void on_resize(WindowResizeEvent* event);
        void on_key(InputKeyEvent* event);
    };

}

CEREAL_REGISTER_TYPE(Birdy3d::FPPlayerController);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Birdy3d::Component, Birdy3d::FPPlayerController);
