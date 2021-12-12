#include "ui/Widget.hpp"

#include "core/Input.hpp"
#include "ui/Canvas.hpp"
#include "ui/Layout.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Triangle.hpp"

namespace Birdy3d::ui {

    Widget::Widget(UIVector pos, UIVector size, Placement placement, std::string name)
        : name(name)
        , pos(pos)
        , size(size)
        , placement(placement) { }

    Rectangle* Widget::add_rectangle(UIVector pos, UIVector size, utils::Color::Name color, Placement placement) {
        std::unique_ptr<Rectangle> rectangle = std::make_unique<Rectangle>(pos, size, color, Shape::OUTLINE, placement);
        Rectangle* ptr = rectangle.get();
        m_shapes.push_back(std::move(rectangle));
        return ptr;
    }

    Rectangle* Widget::add_filled_rectangle(UIVector pos, UIVector size, utils::Color::Name color, Placement placement) {
        std::unique_ptr<Rectangle> rectangle = std::make_unique<Rectangle>(pos, size, color, Shape::FILLED, placement);
        Rectangle* ptr = rectangle.get();
        m_shapes.push_back(std::move(rectangle));
        return ptr;
    }

    Triangle* Widget::add_triangle(UIVector pos, UIVector size, utils::Color::Name color, Placement placement) {
        std::unique_ptr<Triangle> triangle = std::make_unique<Triangle>(pos, size, color, Shape::OUTLINE, placement);
        Triangle* ptr = triangle.get();
        m_shapes.push_back(std::move(triangle));
        return ptr;
    }

    Triangle* Widget::add_filled_triangle(UIVector pos, UIVector size, utils::Color::Name color, Placement placement) {
        std::unique_ptr<Triangle> triangle = std::make_unique<Triangle>(pos, size, color, Shape::FILLED, placement);
        Triangle* ptr = triangle.get();
        m_shapes.push_back(std::move(triangle));
        return ptr;
    }

    Text* Widget::add_text(UIVector pos, std::string text, utils::Color::Name color, Placement placement, float font_size) {
        std::unique_ptr<Text> shape = std::make_unique<Text>(pos, text, color, placement, font_size);
        Text* ptr = shape.get();
        m_shapes.push_back(std::move(shape));
        return ptr;
    }

    void Widget::external_draw() {
        if (hidden)
            return;

        glScissor(m_visible_pos.x, m_visible_pos.y, m_visible_size.x + 2, m_visible_size.y + 2);

        draw();

        if (!m_children_visible)
            return;
        for (const auto& child : m_children)
            child->external_draw();
    }

    void Widget::draw() {
        for (const auto& s : m_shapes)
            s->draw(m_move);
    }

    glm::vec2 Widget::preferred_position(glm::vec2 parentSize, glm::vec2 size) {
        return UIVector::get_relative_position(this->pos, size, parentSize, this->placement);
    }

    glm::vec2 Widget::minimal_size() {
        glm::vec2 children_minsize(m_padding[0] + m_padding[1], m_padding[2] + m_padding[3]);
        if (m_layout && m_children_visible)
            children_minsize += m_layout->minimal_size(m_children);
        return glm::vec2(std::max(children_minsize.x, size.x.to_pixels()), std::max(children_minsize.y, size.y.to_pixels()));
    }

    glm::vec2 Widget::preferred_size(glm::vec2 parentSize) {
        return glm::max(size.to_pixels(parentSize), minimal_size());
    }

    void Widget::arrange(glm::vec2 pos, glm::vec2 size) {
        bool resized = false;
        if (size != m_actual_size)
            resized = true;
        m_actual_pos = pos;
        m_actual_size = size;
        m_move = glm::translate(glm::mat4(1), glm::vec3(m_actual_pos, 0.0f));

        for (const auto& s : m_shapes) {
            s->parent_size(size);
        }

        if (m_layout && m_children_visible)
            m_layout->arrange(m_children, pos + glm::vec2(m_padding[0], m_padding[2]), size - glm::vec2(m_padding[0] + m_padding[1], m_padding[2] + m_padding[3]));

        if (resized)
            on_resize();
    }

    void Widget::set_canvas(Canvas* c) {
        canvas = c;
        for (const auto& child : m_children)
            child->set_canvas(c);
    }

    bool Widget::is_hovering() {
        return canvas->m_hovering_widget == this;
    }

    bool Widget::is_focused() {
        return canvas->m_focused_widget == this;
    }

    bool Widget::was_last_focused() {
        return canvas->m_last_focused_widget == this;
    }

    void Widget::focus() {
        if (canvas->m_focused_widget == this) {
            canvas->m_last_focused_widget = canvas->m_focused_widget;
            return;
        }
        if (canvas->m_focused_widget)
            canvas->m_focused_widget->on_focus_lost();
        canvas->m_last_focused_widget = canvas->m_focused_widget;
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

    bool Widget::contains(glm::vec2 point) const {
        return point.x > m_visible_pos.x && point.y > m_visible_pos.y && point.x < m_visible_pos.x + m_visible_size.x && point.y < m_visible_pos.y + m_visible_size.y;
    }

    bool Widget::update_hover(bool hover) {
        bool success = false;
        if (hidden)
            hover = false;
        for (auto it = m_children.rbegin(); it != m_children.rend(); it++) {
            if ((*it)->update_hover(hover && m_children_visible) && m_children_visible) {
                hover = false;
                success = true;
            }
        }

        if (hidden)
            hover = false;
        if (hover) {
            if (contains(core::Input::cursor_pos())) {
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

    void Widget::update_visible_area(glm::vec2 parent_visible_bottom_left, glm::vec2 parent_visible_top_right) {
        if (hidden)
            return;
        m_visible_pos = glm::vec2(std::max(parent_visible_bottom_left.x, m_actual_pos.x), std::max(parent_visible_bottom_left.y, m_actual_pos.y));
        glm::vec2 actual_pos2 = m_actual_pos + m_actual_size;
        glm::vec2 visible_pos2 = glm::vec2(std::min(parent_visible_top_right.x, actual_pos2.x), std::min(parent_visible_top_right.y, actual_pos2.y));
        m_visible_size = visible_pos2 - m_visible_pos;

        for (const auto& child : m_children)
            child->update_visible_area(m_visible_pos, visible_pos2);
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

    void Widget::add_child(std::shared_ptr<Widget> w) {
        w->parent = this;
        w->set_canvas(canvas);
        m_children.push_back(std::move(w));
    }

    void Widget::clear_children() {
        m_children.clear();
    }

    void Widget::to_foreground(Widget* widget) {
        auto element = std::find_if(m_children.cbegin(), m_children.cend(), [&](const std::shared_ptr<Widget>& w) { return w.get() == widget; });
        if (element == m_children.cend())
            return;
        m_children.splice(m_children.end(), m_children, element);
    }

    void Widget::on_scroll(const events::InputScrollEvent& event) {
        if (parent)
            parent->on_scroll(event);
    }

    void Widget::on_click(const events::InputClickEvent& event) {
        if (parent)
            parent->on_click(event);
    }

    void Widget::on_key(const events::InputKeyEvent& event) {
        if (parent)
            parent->on_key(event);
    }

    void Widget::on_char(const events::InputCharEvent& event) {
        if (parent)
            parent->on_char(event);
    }

    void Widget::add_callback(const std::string& name, CallbackType callback) {
        m_callbacks[name].push_back(callback);
    }

    void Widget::execute_callbacks(const std::string& name) {
        for (auto& callback : m_callbacks[name])
            std::invoke(callback);
    }

    bool Widget::has_callbacks(const std::string& name) {
        return !m_callbacks[name].empty();
    }

}
