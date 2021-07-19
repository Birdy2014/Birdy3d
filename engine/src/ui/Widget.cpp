#include "ui/Widget.hpp"

#include "core/Input.hpp"
#include "ui/Canvas.hpp"
#include "ui/Layout.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Triangle.hpp"

namespace Birdy3d {

    Widget::Widget(UIVector pos, UIVector size, Placement placement, Theme* theme, std::string name)
        : name(name)
        , pos(pos)
        , size(size)
        , placement(placement)
        , theme(theme) { }

    Rectangle* Widget::add_rectangle(UIVector pos, UIVector size, Color color, Placement placement) {
        std::unique_ptr<Rectangle> rectangle = std::make_unique<Rectangle>(pos, size, color, Shape::OUTLINE, placement);
        Rectangle* ptr = rectangle.get();
        m_shapes.push_back(std::move(rectangle));
        return ptr;
    }

    Rectangle* Widget::add_filled_rectangle(UIVector pos, UIVector size, Color color, Placement placement) {
        std::unique_ptr<Rectangle> rectangle = std::make_unique<Rectangle>(pos, size, color, Shape::FILLED, placement);
        Rectangle* ptr = rectangle.get();
        m_shapes.push_back(std::move(rectangle));
        return ptr;
    }

    Triangle* Widget::add_triangle(UIVector pos, UIVector size, Color color) {
        std::unique_ptr<Triangle> triangle = std::make_unique<Triangle>(pos, size, color, Shape::OUTLINE);
        Triangle* ptr = triangle.get();
        m_shapes.push_back(std::move(triangle));
        return ptr;
    }

    Triangle* Widget::add_filled_triangle(UIVector pos, UIVector size, Color color) {
        std::unique_ptr<Triangle> triangle = std::make_unique<Triangle>(pos, size, color, Shape::FILLED);
        Triangle* ptr = triangle.get();
        m_shapes.push_back(std::move(triangle));
        return ptr;
    }

    Text* Widget::add_text(UIVector pos, float fontSize, std::string text, Color color, Placement placement) {
        std::unique_ptr<Text> shape = std::make_unique<Text>(pos, fontSize, text, color, placement, theme->text_renderer());
        Text* ptr = shape.get();
        m_shapes.push_back(std::move(shape));
        return ptr;
    }

    void Widget::draw() {
        if (hidden)
            return;

        glm::mat4 move = normalizedMove();

        for (const auto& s : m_shapes) {
            s->draw(move);
        }

        for (const auto& child : m_children)
            child->draw();
    }

    glm::vec2 Widget::preferredPosition(glm::vec2 parentSize, glm::vec2 size) {
        return Utils::getRelativePosition(this->pos, size, parentSize, this->placement);
    }

    glm::vec2 Widget::minimalSize() {
        glm::vec2 children_minsize(m_padding[0] + m_padding[1], m_padding[2] + m_padding[3]);
        if (m_layout)
            children_minsize += m_layout->minimal_size(m_children);
        return glm::max(children_minsize, size.toPixels());
    }

    glm::vec2 Widget::preferredSize(glm::vec2 parentSize) {
        return glm::max(size.toPixels(parentSize), minimalSize());
    }

    void Widget::arrange(glm::vec2 pos, glm::vec2 size) {
        m_actual_pos = pos;
        m_actual_size = size;

        for (const auto& s : m_shapes) {
            s->parentSize(size);
        }

        if (m_layout)
            m_layout->arrange(m_children, pos + glm::vec2(m_padding[0], m_padding[2]), size - glm::vec2(m_padding[0] + m_padding[1], m_padding[2] + m_padding[3]));
    }

    void Widget::set_canvas(Canvas* c) {
        canvas = c;
        for (const std::unique_ptr<Widget>& child : m_children)
            child->set_canvas(c);
    }

    glm::mat4 Widget::normalizedMove() {
        glm::vec2 viewportSize = Application::getViewportSize();
        glm::mat4 move = glm::ortho(0.0f, viewportSize.x, 0.0f, viewportSize.y);
        return glm::translate(move, glm::vec3(m_actual_pos, 0.0f));
    }

    bool Widget::is_hovering() {
        return canvas->m_hovering_widget == this;
    }

    bool Widget::is_focused() {
        return canvas->m_focused_widget == this;
    }

    void Widget::focus() {
        if (canvas->m_focused_widget)
            canvas->m_focused_widget->on_focus_lost();
        canvas->m_focused_widget = this;
        canvas->m_cursor_grabbed = false;
        canvas->m_focused_widget->on_focus();
    }

    void Widget::grab_cursor() {
        canvas->m_cursor_grabbed = true;
    }

    void Widget::ungrab_cursor() {
        canvas->m_cursor_grabbed = false;
    }

    bool Widget::update_hover(bool hover) {
        bool success = false;
        if (hidden)
            hover = false;
        for (std::list<std::unique_ptr<Widget>>::reverse_iterator it = m_children.rbegin(); it != m_children.rend(); it++) {
            if ((*it)->update_hover(hover)) {
                hover = false;
                success = true;
            }
        }

        if (hidden)
            hover = false;
        if (hover) {
            glm::vec2 cursorPos = Input::cursorPos();
            if (cursorPos.x > m_actual_pos.x && cursorPos.y > m_actual_pos.y && cursorPos.x < m_actual_pos.x + m_actual_size.x && cursorPos.y < m_actual_pos.y + m_actual_size.y) {
                canvas->m_hovering_widget = this;
                return true;
            }
        }
        if (m_hovered_last_frame) {
            on_mouse_leave();
            m_hovered_last_frame = false;
        }
        return success;
    }

    void Widget::on_update() {
        for (auto it = m_children.rbegin(); it != m_children.rend(); it++) {
            (*it)->on_update();
        }
    }

    void Widget::late_update() {
        for (auto it = m_children.rbegin(); it != m_children.rend(); it++)
            (*it)->late_update();
        if (is_hovering() && !m_hovered_last_frame) {
            on_mouse_enter();
            m_hovered_last_frame = true;
        }
    }

    void Widget::add_child(std::unique_ptr<Widget> w) {
        w->parent = this;
        w->set_canvas(canvas);
        m_children.push_back(std::move(w));
    }

    void Widget::toForeground(Widget* widget) {
        auto element = std::find_if(m_children.cbegin(), m_children.cend(), [&](const std::unique_ptr<Widget>& w) { return w.get() == widget; });
        m_children.splice(m_children.end(), m_children, element);
    }

}
