#include "ui/Widget.hpp"

#include "core/Input.hpp"
#include "core/Logger.hpp"
#include "ui/Canvas.hpp"
#include "ui/Layout.hpp"
#include "ui/Painter.hpp"
#include "ui/Theme.hpp"
#include "ui/UIEvent.hpp"
#include <algorithm>
#include <cassert>

namespace Birdy3d::ui {

    void Widget::paint_background(bool outline) const
    {
        auto& theme = core::Application::theme();
        auto background_color = theme.color(utils::Color::Name::BG);
        auto outline_color = theme.color(utils::Color::Name::BORDER);
        Painter::the().paint_rectangle_filled(m_absolute_rect, background_color, outline ? 1 : 0, outline_color);
    }

    void Widget::paint_background(utils::Color const& background_color) const
    {
        Painter::the().paint_rectangle_filled(m_absolute_rect, background_color, 0, utils::Color::NONE);
    }

    void Widget::paint_rectangle_filled(DimRect const& rectangle, utils::Color const& fill_color, unsigned int outline_width, utils::Color const& outline_color) const
    {
        Painter::the().paint_rectangle_filled(rectangle.to_rect(m_absolute_rect), fill_color, outline_width, outline_color);
    }

    void Widget::paint_rectangle_texture(DimRect const& rectangle, render::Texture const& texture) const
    {
        Painter::the().paint_rectangle_texture(rectangle.to_rect(m_absolute_rect), texture);
    }

    void Widget::paint_triangle_filled(DimRect const& rectangle, float orientation, utils::Color const& fill_color) const
    {
        Painter::the().paint_triangle_filled(rectangle.to_rect(m_absolute_rect), orientation, fill_color);
    }

    void Widget::paint_text(Position position, Placement placement, TextDescription const& text) const
    {
        auto relative_position = Position::get_relative_position(position, Size::make_pixels(text.text_size()), m_absolute_rect.size(), placement);
        auto absolute_position = m_absolute_rect.position() + relative_position;
        Painter::the().paint_text(absolute_position, text);
    }

    void Widget::paint_rectangle_filled(Rect const& rectangle, utils::Color const& fill_color, unsigned int outline_width, utils::Color const& outline_color) const
    {
        Painter::the().paint_rectangle_filled(Rect::from_position_and_size(m_absolute_rect.position() + rectangle.position(), rectangle.size()), fill_color, outline_width, outline_color);
    }

    void Widget::paint_rectangle_texture(Rect const& rectangle, render::Texture const& texture) const
    {
        Painter::the().paint_rectangle_texture(Rect::from_position_and_size(m_absolute_rect.position() + rectangle.position(), rectangle.size()), texture);
    }

    void Widget::paint_triangle_filled(Rect const& rectangle, float orientation, utils::Color const& fill_color) const
    {
        Painter::the().paint_triangle_filled(Rect::from_position_and_size(m_absolute_rect.position() + rectangle.position(), rectangle.size()), orientation, fill_color);
    }

    void Widget::paint_text(glm::ivec2 position, TextDescription const& text) const
    {
        Painter::the().paint_text(m_absolute_rect.position() + position, text);
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
            // birdy3d_dbgln("Invalid event type: {}", event.type);
            assert(false);
        }
        if (event.bubbles() && parent)
            parent->notify_event(event);
    }

    void Widget::external_draw()
    {
        if (hidden)
            return;

        // Background
        Painter::the().visible_rectangle(m_visible_area);

        // Self
        draw();

        // Children
        if (m_children_visible) {
            for (auto const& child : m_children)
                child->external_draw();

            // Foreground
            Painter::the().visible_rectangle(m_visible_area);
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

    void Widget::do_layout(Rect const& rect)
    {
        bool resized = rect.size() != m_absolute_rect.size();
        m_absolute_rect = rect;

        if (m_layout && m_children_visible) {
            auto padding_top_left = glm::ivec2(m_padding.left.to_pixels(), m_padding.top.to_pixels());
            auto padding_size = glm::ivec2(m_padding.left.to_pixels() + m_padding.right.to_pixels(), m_padding.top.to_pixels() + m_padding.bottom.to_pixels());
            m_layout->arrange(m_children, m_absolute_rect.position() + padding_top_left, rect.size() - padding_size);
        }

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
        return m_visible_area.contains(point);
    }

    bool Widget::update_hover()
    {
        if (hidden)
            return false;

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

    void Widget::update_visible_area(Rect const& parent_visible_area)
    {
        if (hidden)
            return;

        m_visible_area = m_absolute_rect;
        m_visible_area.shrink_to(parent_visible_area);

        for (auto const& child : m_children)
            child->update_visible_area(m_visible_area);
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
