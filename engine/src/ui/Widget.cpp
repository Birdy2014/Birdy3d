#include "ui/Widget.hpp"

#include "core/Input.hpp"
#include "core/Logger.hpp"
#include "ui/Canvas.hpp"
#include "ui/Layout.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Triangle.hpp"
#include "ui/UIEvent.hpp"
#include <algorithm>
#include <cassert>

namespace Birdy3d::ui {

    Rectangle* Widget::add_rectangle(Position pos, Size size, utils::Color::Name color, Placement placement)
    {
        std::unique_ptr<Rectangle> rectangle = std::make_unique<Rectangle>(pos, size, color, Shape::OUTLINE, placement);
        Rectangle* ptr = rectangle.get();
        m_shapes.push_back(std::move(rectangle));
        return ptr;
    }

    Rectangle* Widget::add_filled_rectangle(Position pos, Size size, utils::Color::Name color, Placement placement)
    {
        std::unique_ptr<Rectangle> rectangle = std::make_unique<Rectangle>(pos, size, color, Shape::FILLED, placement);
        Rectangle* ptr = rectangle.get();
        m_shapes.push_back(std::move(rectangle));
        return ptr;
    }

    Triangle* Widget::add_triangle(Position pos, Size size, utils::Color::Name color, Placement placement)
    {
        std::unique_ptr<Triangle> triangle = std::make_unique<Triangle>(pos, size, color, Shape::OUTLINE, placement);
        Triangle* ptr = triangle.get();
        m_shapes.push_back(std::move(triangle));
        return ptr;
    }

    Triangle* Widget::add_filled_triangle(Position pos, Size size, utils::Color::Name color, Placement placement)
    {
        std::unique_ptr<Triangle> triangle = std::make_unique<Triangle>(pos, size, color, Shape::FILLED, placement);
        Triangle* ptr = triangle.get();
        m_shapes.push_back(std::move(triangle));
        return ptr;
    }

    Text* Widget::add_text(Position pos, std::string text, utils::Color::Name color, Placement placement, int font_size)
    {
        std::unique_ptr<Text> shape = std::make_unique<Text>(pos, text, color, placement, font_size);
        Text* ptr = shape.get();
        m_shapes.push_back(std::move(shape));
        return ptr;
    }

    void Widget::notify_event(UIEvent& event)
    {
        switch (event.type) {
        case UIEvent::SCROLL: {
            auto& casted_event = static_cast<ScrollEvent&>(event);
            on_scroll(casted_event);
            execute_callbacks("on_scroll", casted_event);
            break;
        }
        case UIEvent::CLICK: {
            auto& casted_event = static_cast<ClickEvent&>(event);
            on_click(casted_event);
            execute_callbacks("on_click", casted_event);
            break;
        }
        case UIEvent::KEY: {
            auto& casted_event = static_cast<KeyEvent&>(event);
            on_key(casted_event);
            execute_callbacks("on_key", casted_event);
            break;
        }
        case UIEvent::CHAR: {
            auto& casted_event = static_cast<CharEvent&>(event);
            on_char(casted_event);
            execute_callbacks("on_char", casted_event);
            break;
        }
        case UIEvent::MOUSE_ENTER: {
            auto& casted_event = static_cast<MouseEnterEvent&>(event);
            on_mouse_enter(casted_event);
            execute_callbacks("on_mouse_enter", casted_event);
            break;
        }
        case UIEvent::MOUSE_LEAVE: {
            auto& casted_event = static_cast<MouseLeaveEvent&>(event);
            on_mouse_leave(casted_event);
            execute_callbacks("on_mouse_leave", casted_event);
            break;
        }
        case UIEvent::FOCUS: {
            auto& casted_event = static_cast<FocusEvent&>(event);
            on_focus(casted_event);
            execute_callbacks("on_focus", casted_event);
            break;
        }
        case UIEvent::FOCUS_LOST: {
            auto& casted_event = static_cast<FocusLostEvent&>(event);
            on_focus_lost(casted_event);
            execute_callbacks("on_focus_lost", casted_event);
            break;
        }
        case UIEvent::RESIZE: {
            auto& casted_event = static_cast<ResizeEvent&>(event);
            on_resize(casted_event);
            execute_callbacks("on_resize", casted_event);
            break;
        }
        case UIEvent::DROP: {
            auto& casted_event = static_cast<DropEvent&>(event);
            on_drop(casted_event);
            execute_callbacks("on_drop", casted_event);
            break;
        }
        default:
            birdy3d_dbgln("Invalid event type: {}", event.type);
            assert(false);
        }
        if (event.bubbles() && parent)
            parent->notify_event(event);
    }

    void Widget::external_draw()
    {
        if (hidden)
            return;

        // Transform to OpenGL coordinates
        auto viewport_y = core::Application::get_viewport_size().y;
        auto visible_pos_bottom = m_visible_pos.y + m_visible_size.y;
        auto visible_pos_bottom_moved_origin = viewport_y - visible_pos_bottom;

        // Background
        assert(m_visible_size.x >= 0 && m_visible_size.y >= 0);
        glScissor(m_visible_pos.x, visible_pos_bottom_moved_origin - 1, m_visible_size.x + 2, m_visible_size.y + 2);

        if (m_shapes_visible) {
            for (auto const& s : m_shapes) {
                if (!s->in_foreground)
                    s->draw(m_move);
            }
        }

        // Children
        if (m_children_visible) {
            for (auto const& child : m_children)
                child->external_draw();

            // Foreground
            glScissor(m_visible_pos.x, visible_pos_bottom_moved_origin - 1, m_visible_size.x + 2, m_visible_size.y + 2);
        }

        draw();

        if (m_shapes_visible) {
            for (auto const& s : m_shapes) {
                if (s->in_foreground)
                    s->draw(m_move);
            }
        }
    }

    void Widget::draw() { }

    glm::ivec2 Widget::preferred_position(glm::ivec2 parent_size, glm::ivec2 size)
    {
        return Position::get_relative_position(position, Size::make_pixels(size), parent_size, placement);
    }

    glm::ivec2 Widget::minimal_size()
    {
        glm::ivec2 children_minsize(m_padding.left.to_pixels() + m_padding.right.to_pixels(), m_padding.top.to_pixels() + m_padding.bottom.to_pixels());
        if (m_layout && m_children_visible)
            children_minsize += m_layout->minimal_size(m_children);
        return glm::ivec2(std::max(children_minsize.x, size.x.to_pixels()), std::max(children_minsize.y, size.y.to_pixels()));
    }

    glm::ivec2 Widget::preferred_size(glm::ivec2 parent_size)
    {
        return glm::max(size.to_pixels(parent_size), minimal_size());
    }

    void Widget::arrange(glm::ivec2 pos, glm::ivec2 size)
    {
        bool resized = false;
        if (size != m_actual_size)
            resized = true;
        m_actual_pos = pos;
        m_actual_size = size;
        m_move = glm::translate(glm::mat4(1), glm::vec3(m_actual_pos, 0.0f));

        for (auto const& s : m_shapes) {
            s->parent_size(size);
        }

        if (m_layout && m_children_visible)
            m_layout->arrange(m_children, pos + glm::ivec2(m_padding.left.to_pixels(), m_padding.top.to_pixels()), size - glm::ivec2(m_padding.left.to_pixels() + m_padding.right.to_pixels(), m_padding.top.to_pixels() + m_padding.bottom.to_pixels()));

        if (resized) {
            auto event = ResizeEvent{};
            notify_event(event);
        }
    }

    void Widget::set_canvas(Canvas* c)
    {
        canvas = c;
        for (auto const& child : m_children)
            child->set_canvas(c);
    }

    bool Widget::is_hovering()
    {
        return canvas->hovering_widget() == this;
    }

    bool Widget::is_focused()
    {
        return canvas->focused_widget() == this;
    }

    bool Widget::was_last_focused()
    {
        return canvas->last_focused_widget() == this;
    }

    void Widget::focus()
    {
        canvas->set_focused(this);
    }

    void Widget::grab_cursor()
    {
        canvas->set_cursor_grabbed(this, true);
    }

    void Widget::ungrab_cursor()
    {
        canvas->set_cursor_grabbed(this, false);
    }

    bool Widget::contains(glm::ivec2 point) const
    {
        auto local_point = point - m_actual_pos;
        return (point.x > m_visible_pos.x && point.y > m_visible_pos.y && point.x < m_visible_pos.x + m_visible_size.x && point.y < m_visible_pos.y + m_visible_size.y)
            && (std::ranges::find_if(m_shapes, [&local_point](auto const& shape) { return shape->contains(local_point); }) != m_shapes.end());
    }

    bool Widget::update_hover()
    {
        if (hidden)
            return false;

        // foreground shapes
        for (auto const& shape : m_shapes) {
            if (shape->in_foreground && shape->contains(core::Input::cursor_pos_int() - m_actual_pos)) {
                canvas->set_hovering(this);
                return true;
            }
        }

        if (m_children_visible) {
            for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
                if ((*it)->update_hover()) {
                    return true;
                }
            }
        }

        if (canvas == this || contains(core::Input::cursor_pos())) {
            canvas->set_hovering(this);
            return true;
        }

        return false;
    }

    void Widget::update_visible_area(glm::ivec2 parent_visible_top_left, glm::ivec2 parent_visible_bottom_right)
    {
        assert(parent_visible_top_left.x <= parent_visible_bottom_right.x && parent_visible_top_left.y <= parent_visible_bottom_right.y);
        assert(m_actual_size.x >= 0 && m_actual_size.y >= 0);

        if (hidden)
            return;

        m_visible_pos = glm::ivec2(std::max(parent_visible_top_left.x, m_actual_pos.x), std::max(parent_visible_top_left.y, m_actual_pos.y));
        glm::ivec2 actual_bottom_right = m_actual_pos + m_actual_size;
        glm::ivec2 visible_bottom_right = glm::ivec2(std::min(parent_visible_bottom_right.x, actual_bottom_right.x), std::min(parent_visible_bottom_right.y, actual_bottom_right.y));
        m_visible_size = glm::ivec2(std::max(visible_bottom_right.x - m_visible_pos.x, 0), std::max(visible_bottom_right.y - m_visible_pos.y, 0));

        for (auto const& child : m_children)
            child->update_visible_area(m_visible_pos, visible_bottom_right);
    }

    void Widget::on_update()
    {
        for (auto it = m_children.rbegin(); it != m_children.rend(); it++) {
            (*it)->on_update();
        }
    }

    // TODO: Remove?
    void Widget::late_update()
    {
        for (auto it = m_children.rbegin(); it != m_children.rend(); it++)
            (*it)->late_update();
    }

    void Widget::add_child(std::shared_ptr<Widget> w)
    {
        w->parent = this;
        w->set_canvas(canvas);
        m_children.push_back(std::move(w));
    }

    void Widget::clear_children()
    {
        m_children.clear();
    }

    void Widget::to_foreground(Widget* widget)
    {
        auto element = std::find_if(m_children.cbegin(), m_children.cend(), [&](std::shared_ptr<Widget> const& w) { return w.get() == widget; });
        if (element == m_children.cend())
            return;
        m_children.splice(m_children.end(), m_children, element);
    }

    void Widget::on_scroll(ScrollEvent&) { }

    void Widget::on_click(ClickEvent&) { }

    void Widget::on_key(KeyEvent&) { }

    void Widget::on_char(CharEvent&) { }

    void Widget::add_callback(std::string const& name, CallbackType callback)
    {
        m_callbacks[name].push_back(callback);
    }

    void Widget::execute_callbacks(std::string const& name, UIEvent& event)
    {
        for (auto& callback : m_callbacks[name])
            std::invoke(callback, event);
    }

    bool Widget::has_callbacks(std::string const& name)
    {
        return !m_callbacks[name].empty();
    }

}
