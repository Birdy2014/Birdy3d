#include "ui/Layout.hpp"

#include "ui/Shape.hpp"
#include "ui/TextRenderer.hpp"

namespace Birdy3d {

    Layout::Layout(UIVector pos, UIVector size, Placement placement, Theme* theme, std::string name)
        : Widget(pos, size, placement, theme, name) { }

    void Layout::addChild(Widget* w) {
        w->parent = this;
        w->set_canvas(canvas);
        m_children.push_back(w);
    }

    void Layout::toForeground(Widget* w) {
        std::list<Widget*>::iterator element = std::find(m_children.begin(), m_children.end(), w);
        m_children.splice(m_children.end(), m_children, element);
    }

    void Layout::draw() {
        if (hidden)
            return;
        Widget::draw();
        for (Widget* w : m_children) {
            w->draw();
        }
    }

    void Layout::set_canvas(Canvas* c) {
        Widget::set_canvas(c);

        for (Widget* child : m_children)
            child->set_canvas(c);
    }

    bool Layout::update_hover(bool hover) {
        bool success = false;
        if (hidden)
            hover = false;
        for (std::list<Widget*>::reverse_iterator it = m_children.rbegin(); it != m_children.rend(); it++) {
            if ((*it)->update_hover(hover)) {
                hover = false;
                success = true;
            }
        }
        return Widget::update_hover(hover) || success;
    }

    void Layout::late_update() {
        for (std::list<Widget*>::reverse_iterator it = m_children.rbegin(); it != m_children.rend(); it++) {
            (*it)->late_update();
        }

        Widget::late_update();
    }

    void Layout::on_update() {
        for (std::list<Widget*>::reverse_iterator it = m_children.rbegin(); it != m_children.rend(); it++) {
            (*it)->on_update();
        }

        Widget::on_update();
    }

}
