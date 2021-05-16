#pragma once

#include "core/Application.hpp"
#include "ui/Widget.hpp"
#include "ui/widgets/AbsoluteLayout.hpp"

namespace Birdy3d {

    class Canvas : public AbsoluteLayout {
    public:
        bool updated = false;

        Canvas()
            : AbsoluteLayout(0_px, 100_p) {};

        void update() {
            if (!hidden) {
                updated = true;
                glm::mat4 m(1);
                glm::vec2 viewport = Application::getViewportSize();
                AbsoluteLayout::arrange(m, viewport);
                AbsoluteLayout::updateEvents();
            }
        }

        void draw() override {
            if (updated)
                AbsoluteLayout::draw();
        }
    };

}
