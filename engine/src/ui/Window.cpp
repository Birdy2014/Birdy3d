#include "ui/Window.hpp"

#include "core/Input.hpp"
#include "ui/Layout.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d::ui {

    Window::Window(Options options)
        : Container(options)
    {
        m_padding = {
            .left = Dimension::make_pixels(border_size),
            .right = Dimension::make_pixels(border_size),
            .top = 1_em,
            .bottom = Dimension::make_pixels(border_size)};
        m_close_button_rect = DimRect::from_position_and_size({-4_px, 4_px}, {1_em - 8_px}, Placement::TOP_RIGHT);
    }

    void Window::to_foreground()
    {
        parent->to_foreground(this);
    }

    glm::ivec2 Window::minimal_size()
    {
        glm::ivec2 layout_minsize = Widget::minimal_size();
        int min_width = m_title.text_size().x + m_close_button_rect.size().x.to_pixels() + border_size * 2 + 14;
        return glm::ivec2(std::max(layout_minsize.x, min_width), layout_minsize.y);
    }

    glm::ivec2 Window::minimal_window_size()
    {
        glm::ivec2 layout_minsize(m_padding.left.to_pixels() + m_padding.right.to_pixels(), m_padding.top.to_pixels() + m_padding.bottom.to_pixels());
        if (m_layout && m_children_visible)
            layout_minsize += m_layout->minimal_size(m_children);
        int min_width = m_title.text_size().x + m_close_button_rect.size().x.to_pixels() + border_size * 2 + 14;
        return glm::ivec2(std::max(layout_minsize.x, min_width), layout_minsize.y);
    }

    void Window::draw_titlebar()
    {
        auto bg_color = core::Application::theme().color(utils::Color::Name::BG);
        auto color_red = core::Application::theme().color(utils::Color::Name::RED);
        paint_rectangle_filled(DimRect::from_position_and_size(Position(0_px), Size(100_pc, 1_em), Placement::TOP_LEFT), bg_color);
        paint_rectangle_filled(m_close_button_rect, color_red);
        paint_text(glm::ivec2(10, 3), m_title);
    }

    void Window::draw()
    {
        paint_background(true);
        draw_titlebar();
        // HACK: Ugly workaround for titlebar drawing over border
        paint_rectangle_filled(Rect::from_position_and_size(glm::ivec2(0), m_absolute_rect.size()), utils::Color::NONE, 1, core::Application::theme().color(utils::Color::Name::BORDER));
    }

    void Window::on_update()
    {
        Widget::on_update();

        m_dragged = false;

        if (!is_hovering())
            return;

        glm::ivec2 local_cursor_pos = core::Input::cursor_pos_int() - m_absolute_rect.position();

        m_hover_drag = false;
        m_hover_resize_x_left = false;
        m_hover_resize_x_right = false;
        m_hover_resize_y_top = false;
        m_hover_resize_y_bottom = false;

        if (!m_dragging && !m_resize_x_left && !m_resize_x_right && !m_resize_y_top && !m_resize_y_bottom && m_close_button_rect.contains(core::Input::cursor_pos_int(), m_absolute_rect)) {
            core::Input::set_cursor(core::Input::CURSOR_HAND);
            return;
        }

        if (local_cursor_pos.x < border_size)
            m_hover_resize_x_left = true;
        if (local_cursor_pos.x > m_absolute_rect.width() - border_size)
            m_hover_resize_x_right = true;
        if (local_cursor_pos.y < border_size)
            m_hover_resize_y_top = true;
        if (local_cursor_pos.y > m_absolute_rect.height() - border_size)
            m_hover_resize_y_bottom = true;

        if (local_cursor_pos.y <= core::Application::theme().line_height() && local_cursor_pos.y > border_size && local_cursor_pos.x >= border_size && local_cursor_pos.x <= m_absolute_rect.width() - border_size)
            m_hover_drag = true;

        // Set cursor
        if (m_dragging)
            core::Input::set_cursor(core::Input::CURSOR_MOVE);
        else if (m_resize_x_left && m_resize_y_top)
            core::Input::set_cursor(core::Input::CURSOR_TOP_LEFT_RESIZE);
        else if (m_resize_x_right && m_resize_y_top)
            core::Input::set_cursor(core::Input::CURSOR_TOP_RIGHT_RESIZE);
        else if (m_resize_x_left && m_resize_y_bottom)
            core::Input::set_cursor(core::Input::CURSOR_BOTTOM_LEFT_RESIZE);
        else if (m_resize_x_right && m_resize_y_bottom)
            core::Input::set_cursor(core::Input::CURSOR_BOTTOM_RIGHT_RESIZE);
        else if (m_resize_x_left || m_resize_x_right)
            core::Input::set_cursor(core::Input::CURSOR_HRESIZE);
        else if (m_resize_y_top || m_resize_y_bottom)
            core::Input::set_cursor(core::Input::CURSOR_VRESIZE);
        else if (m_hover_drag)
            core::Input::set_cursor(core::Input::CURSOR_MOVE);
        else if (m_hover_resize_x_left && m_hover_resize_y_top)
            core::Input::set_cursor(core::Input::CURSOR_TOP_LEFT_RESIZE);
        else if (m_hover_resize_x_right && m_hover_resize_y_top)
            core::Input::set_cursor(core::Input::CURSOR_TOP_RIGHT_RESIZE);
        else if (m_hover_resize_x_left && m_hover_resize_y_bottom)
            core::Input::set_cursor(core::Input::CURSOR_BOTTOM_LEFT_RESIZE);
        else if (m_hover_resize_x_right && m_hover_resize_y_bottom)
            core::Input::set_cursor(core::Input::CURSOR_BOTTOM_RIGHT_RESIZE);
        else if (m_hover_resize_x_left || m_hover_resize_x_right)
            core::Input::set_cursor(core::Input::CURSOR_HRESIZE);
        else if (m_hover_resize_y_top || m_hover_resize_y_bottom)
            core::Input::set_cursor(core::Input::CURSOR_VRESIZE);
        else
            core::Input::set_cursor(core::Input::CURSOR_DEFAULT);

        // Move and resize
        if (m_dragging) {
            position = Position::make_pixels(core::Input::cursor_pos() - m_cursor_down_local_pos);
            m_dragged = true;
        }
        auto minsize = Size::make_pixels(minimal_window_size());
        if (m_resize_x_left) {
            auto new_x = Dimension::make_pixels(core::Input::cursor_pos().x - m_cursor_down_local_pos.x);
            if (size.x + position.x - new_x < minsize.x) {
                new_x = size.x + position.x - minsize.x;
            }
            size.x += position.x - new_x;
            position.x = new_x;
        }
        if (m_resize_x_right) {
            size.x = Dimension::make_pixels(local_cursor_pos.x);
        }
        if (m_resize_y_top) {
            auto new_y = Dimension::make_pixels(local_cursor_pos.y);
            if (size.y + position.y - new_y < minsize.y) {
                new_y = size.y + position.y - minsize.y;
            }
            size.y += position.y - new_y;
            position.y = new_y;
        }
        if (m_resize_y_bottom) {
            size.y = Dimension::make_pixels(local_cursor_pos.y);
        }
        m_absolute_rect.position(position.to_pixels());
        m_absolute_rect.size(glm::ivec2(std::max(size.x.to_pixels(), minimal_size().x), std::max(size.y.to_pixels(), minimal_size().y)));
    }

    void Window::on_click(ClickEvent& event)
    {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        event.handled();

        if (event.action == GLFW_PRESS) {
            to_foreground();
            m_cursor_down_local_pos = core::Input::cursor_pos() - glm::vec2(m_absolute_rect.position());
        }

        if (event.action == GLFW_RELEASE) {
            ungrab_cursor();
            m_dragging = false;
            m_resize_x_left = false;
            m_resize_x_right = false;
            m_resize_y_top = false;
            m_resize_y_bottom = false;
            size = Size::make_pixels(m_absolute_rect.size());
            return;
        }

        if (m_close_button_rect.contains(core::Input::cursor_pos_int(), m_absolute_rect)) {
            if (callback_close)
                callback_close();
            return;
        }

        grab_cursor();

        // Moving
        if (m_hover_drag) {
            m_dragging = true;
            return;
        }

        // Resizing
        if (m_hover_resize_x_left)
            m_resize_x_left = true;
        if (m_hover_resize_x_right)
            m_resize_x_right = true;
        if (m_hover_resize_y_top)
            m_resize_y_top = true;
        if (m_hover_resize_y_bottom)
            m_resize_y_bottom = true;
        return;
    }

    void Window::on_mouse_leave(MouseLeaveEvent&)
    {
        core::Input::set_cursor(core::Input::CURSOR_DEFAULT);
    }

}
