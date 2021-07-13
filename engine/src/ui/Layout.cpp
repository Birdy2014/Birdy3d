#include "ui/Layout.hpp"

#include "ui/Shape.hpp"
#include "ui/TextRenderer.hpp"

namespace Birdy3d {

    Layout::Layout(UIVector pos, UIVector size, Placement placement, Theme* theme, std::string name)
        : Widget(pos, size, placement, theme, name) { }

    void Layout::add_child(std::unique_ptr<Widget> w) {
        w->parent = this;
        w->set_canvas(canvas);
        m_children.push_back(std::move(w));
    }

    void Layout::toForeground(Widget* widget) {
        std::list<std::unique_ptr<Widget>>::const_iterator element = std::find_if(m_children.cbegin(), m_children.cend(), [&](const std::unique_ptr<Widget>& w){ return w.get() == widget; });
        m_children.splice(m_children.end(), m_children, element);
    }

    void Layout::draw() {
        if (hidden)
            return;
        Widget::draw();
        for (const std::unique_ptr<Widget>& w : m_children) {
            w->draw();
        }
    }

    void Layout::set_canvas(Canvas* c) {
        Widget::set_canvas(c);

        for (const std::unique_ptr<Widget>& child : m_children)
            child->set_canvas(c);
    }

    bool Layout::update_hover(bool hover) {
        bool success = false;
        if (hidden)
            hover = false;
        for (std::list<std::unique_ptr<Widget>>::reverse_iterator it = m_children.rbegin(); it != m_children.rend(); it++) {
            if ((*it)->update_hover(hover)) {
                hover = false;
                success = true;
            }
        }
        return Widget::update_hover(hover) || success;
    }

    void Layout::late_update() {
        for (std::list<std::unique_ptr<Widget>>::reverse_iterator it = m_children.rbegin(); it != m_children.rend(); it++) {
            (*it)->late_update();
        }

        Widget::late_update();
    }

    void Layout::on_update() {
        for (std::list<std::unique_ptr<Widget>>::reverse_iterator it = m_children.rbegin(); it != m_children.rend(); it++) {
            (*it)->on_update();
        }

        Widget::on_update();
    }

}
