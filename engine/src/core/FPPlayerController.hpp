#pragma once

#include "core/Component.hpp"

namespace Birdy3d {

    class Camera;
    class InputKeyEvent;
    class Widget;
    class WindowResizeEvent;

    class FPPlayerController : public Component {
    public:
        FPPlayerController();
        void start() override;
        void cleanup() override;
        void update() override;

    private:
        Camera* cam;
        Widget* menu;

        void onResize(WindowResizeEvent* event);
        void onKey(InputKeyEvent* event);
    };

}
