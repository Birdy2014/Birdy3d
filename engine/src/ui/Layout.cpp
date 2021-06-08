#pragma once

#include "ui/Layout.hpp"
#include "ui/Shape.hpp"
#include "ui/TextRenderer.hpp"

namespace Birdy3d {

    Layout::Layout(UIVector pos, UIVector size, Placement placement, Theme* theme, std::string name)
        : Widget(pos, size, placement, theme, name) { }

    void Layout::addChild(Widget* w) {
        w->parent = this;
        children.push_back(w);
    }

    void Layout::toForeground(Widget* w) {
        std::list<Widget*>::iterator element = std::find(children.begin(), children.end(), w);
        children.splice(children.end(), children, element);
    }

    void Layout::draw() {
        if (hidden)
            return;
        Widget::draw();
        for (Widget* w : children) {
            w->draw();
        }
    }

    void Layout::lateUpdate() {
        for (std::list<Widget*>::reverse_iterator it = children.rbegin(); it != children.rend(); it++) {
            (*it)->lateUpdate();
        }

        Widget::lateUpdate();
    }

    bool Layout::onScroll(InputScrollEvent* event, bool hover) {
        for (std::list<Widget*>::reverse_iterator it = children.rbegin(); it != children.rend(); it++) {
            if ((*it)->notifyEvent(EventType::SCROLL, event, hover))
                hover = false;
        }

        return !hover;
    }

    bool Layout::onClick(InputClickEvent* event, bool hover) {
        for (std::list<Widget*>::reverse_iterator it = children.rbegin(); it != children.rend(); it++) {
            if ((*it)->notifyEvent(EventType::CLICK, event, hover))
                hover = false;
        }

        return !hover;
    }

    bool Layout::onKey(InputKeyEvent* event, bool hover) {
        for (std::list<Widget*>::reverse_iterator it = children.rbegin(); it != children.rend(); it++) {
            if ((*it)->notifyEvent(EventType::KEY, event, hover))
                hover = false;
        }

        return !hover;
    }

    bool Layout::onChar(InputCharEvent* event, bool hover) {
        for (std::list<Widget*>::reverse_iterator it = children.rbegin(); it != children.rend(); it++) {
            if ((*it)->notifyEvent(EventType::CHAR, event, hover))
                hover = false;
        }

        return !hover;
    }

    bool Layout::update(bool hover) {
        for (std::list<Widget*>::reverse_iterator it = children.rbegin(); it != children.rend(); it++) {
            if ((*it)->notifyEvent(EventType::UPDATE, nullptr, hover))
                hover = false;
        }

        return !hover;
    }

}
