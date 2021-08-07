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
        FPPlayerController();
        void start() override;
        void cleanup() override;
        void update() override;

    private:
        std::weak_ptr<Camera> cam;
        std::weak_ptr<Widget> menu;

        void onResize(WindowResizeEvent* event);
        void onKey(InputKeyEvent* event);
    };

}
