#pragma once

#include "scene/Component.hpp"
#include <memory>

namespace Birdy3d {

    class Camera;
    class InputKeyEvent;
    class Widget;
    class WindowResizeEvent;

    class FPPlayerController : public Component {
    public:
        std::weak_ptr<GameObject> flashlight;

        FPPlayerController();
        void start() override;
        void cleanup() override;
        void update() override;

        template <class Archive>
        void serialize(Archive& ar) {
            ar(CEREAL_NVP(flashlight));
        }

    private:
        std::weak_ptr<Camera> cam;
        std::weak_ptr<Widget> menu;

        void onResize(WindowResizeEvent* event);
        void onKey(InputKeyEvent* event);
    };

}

CEREAL_REGISTER_TYPE(Birdy3d::FPPlayerController);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Birdy3d::Component, Birdy3d::FPPlayerController);
