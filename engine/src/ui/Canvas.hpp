#pragma once

#include "core/Application.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d {

    class Canvas {
    public:
        Widget* child;
        bool hidden = false;
        bool updated = false;

        Canvas() {};

        void update() {
            if (!hidden) {
                updated = true;
                glm::mat4 m(1);
                glm::vec2 viewport = Application::getViewportSize();
                m = glm::translate(m, glm::vec3(child->preferredPosition(viewport), 1.0f));
                child->arrange(m, child->preferredSize(viewport));
                child->updateEvents();
            }
        }

        void draw() {
            if (!hidden && updated)
                child->draw();
        }
    };

}
