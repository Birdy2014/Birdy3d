#pragma once

#include "ui/Layout.hpp"
#include "ui/Shape.hpp"
#include "ui/TextRenderer.hpp"

namespace Birdy3d {

    Layout::Layout(UIVector pos, UIVector size, Placement placement)
        : Widget(pos, size, placement) { }

    void Layout::addChild(Widget* w) {
        children.push_back(w);
    }

    void Layout::draw() {
        if (hidden)
            return;
        Widget::draw();
        for (Widget* w : children) {
            w->draw();
        }
    }

    bool Layout::updateEvents(bool hidden) {
        for (Widget* w : this->children) {
            if (w->updateEvents(hidden || this->hidden))
                return true;
        }

        return Widget::updateEvents(hidden || this->hidden);
    }

}
