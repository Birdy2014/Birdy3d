#pragma once

#include "core/Application.hpp"
#include "ui/Layout.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d {

    class Canvas : public Layout {
    public:
        bool updated = false;

        Canvas()
            : Layout(0_px, 100_p) {};

        void update() {
            if (!hidden) {
                updated = true;
                glm::mat4 m(1);
                glm::vec2 viewport = Application::getViewportSize();
                Layout::arrange(m, viewport);
                Layout::updateEvents();
            }
        }

        void draw() override {
            if (updated)
                Layout::draw();
        }
    };

}
