#ifndef BIRDY3D_CANVAS_HPP
#define BIRDY3D_CANVAS_HPP

#include "core/Application.hpp"
#include "ui/Widget.hpp"

class Canvas {
public:
    Widget *child;
    bool hidden = false;

    Canvas() {};

    void update() {
        if (!hidden) {
            glm::mat4 m(1);
            glm::vec2 viewport = Application::getViewportSize();
            m = glm::translate(m, glm::vec3(child->pixelPosition(viewport), 1.0f));
            child->arrange(m, child->pixelSize(viewport));
            child->updateEvents();
        }
    }

    void draw() {
        if (!hidden)
            child->draw();
    }
};

#endif
