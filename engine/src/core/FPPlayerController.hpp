#pragma once

#include "core/Component.hpp"

namespace Birdy3d {

    class Camera;
    class WindowResizeEvent;

    class FPPlayerController : public Component {
    public:
        FPPlayerController();
        void start() override;
        void cleanup() override;
        void update() override;

    private:
        Camera* cam;
        bool hiddenStatusUpdated = false;

        void onResize(WindowResizeEvent* event);
    };

}
