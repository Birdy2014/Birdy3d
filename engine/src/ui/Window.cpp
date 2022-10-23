#include "ui/Window.hpp"

#include "core/Input.hpp"
#include "ui/Layout.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d::ui {

    Window::Window(Options options)
        : Container(options) {
        m_padding = {
            .left = Dimension::make_pixels(BORDER_SIZE),
            .right = Dimension::make_pixels(BORDER_SIZE),
            .top = 1_em,
            .bottom = Dimension::make_pixels(BORDER_SIZE)
        };
        add_filled_rectangle(0_px, { 100_pc, 100_pc - 1_em }, utils::Color::Name::BG, Placement::BOTTOM_LEFT);
        add_filled_rectangle(0_px, { 100_pc, 1_em }, utils::Color::Name::BG_TITLE_BAR, Placement::TOP_LEFT);
        add_rectangle(0_px, 100_pc, utils::Color::Name::BORDER);
        m_close_button = add_filled_rectangle({ -4_px, 4_px }, { 1_em - 8_px }, utils::Color::Name::RED, Placement::TOP_RIGHT);
        m_title = add_text({ 10_px, 3_px }, "", utils::Color::Name::FG, Placement::TOP_LEFT);
    }

    void Window::to_foreground() {
        parent->to_foreground(this);
    }

    glm::ivec2 Window::minimal_size() {
        glm::ivec2 layout_minsize = Widget::minimal_size();
        int min_width = m_title->size().x.to_pixels() + m_close_button->size().x.to_pixels() + BORDER_SIZE * 2 + 14;
        return glm::ivec2(std::max(layout_minsize.x, min_width), layout_minsize.y);
    }

    glm::ivec2 Window::minimal_window_size() {
        glm::ivec2 layout_minsize(m_padding.left.to_pixels() + m_padding.right.to_pixels(), m_padding.top.to_pixels() + m_padding.bottom.to_pixels());
        if (m_layout && m_children_visible)
            layout_minsize += m_layout->minimal_size(m_children);
        int min_width = m_title->size().x.to_pixels() + m_close_button->size().x.to_pixels() + BORDER_SIZE * 2 + 14;
        return glm::ivec2(std::max(layout_minsize.x, min_width), layout_minsize.y);
    }

    void Window::on_update() {
        Widget::on_update();

        m_dragged = false;

        if (!is_hovering())
            return;

        glm::ivec2 local_cursor_pos = core::Input::cursor_pos_int() - m_actual_pos;

        m_hover_drag = false;
        m_hover_resize_x_left = false;
        m_hover_resize_x_right = false;
        m_hover_resize_y_top = false;
        m_hover_resize_y_bottom = false;

        if (!m_dragging && !m_resize_x_left && !m_resize_x_right && !m_resize_y_top && !m_resize_y_bottom && m_close_button->contains(local_cursor_pos)) {
            core::Input::set_cursor(core::Input::CURSOR_HAND);
            return;
        }

        if (local_cursor_pos.x < BORDER_SIZE)
            m_hover_resize_x_left = true;
        if (local_cursor_pos.x > m_actual_size.x - BORDER_SIZE)
            m_hover_resize_x_right = true;
        if (local_cursor_pos.y < BORDER_SIZE)
            m_hover_resize_y_top = true;
        if (local_cursor_pos.y > m_actual_size.y - BORDER_SIZE)
            m_hover_resize_y_bottom = true;

        if (local_cursor_pos.y <= core::Application::theme().line_height() && local_cursor_pos.y > BORDER_SIZE && local_cursor_pos.x >= BORDER_SIZE && local_cursor_pos.x <= m_actual_size.x - BORDER_SIZE)
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
            size.x = Dimension::make_pixels(core::Input::cursor_pos().x - m_actual_pos.x);
        }
        if (m_resize_y_top) {
            auto new_y = Dimension::make_pixels(core::Input::cursor_pos().y - m_cursor_down_local_pos.y);
            if (size.y + position.y - new_y < minsize.y) {
                new_y = size.y + position.y - minsize.y;
            }
            size.y += position.y - new_y;
            position.y = new_y;
        }
        if (m_resize_y_bottom) {
            size.y = Dimension::make_pixels(core::Input::cursor_pos().y - m_actual_pos.y);
        }
        m_actual_pos = position.to_pixels();
        m_actual_size = glm::ivec2(std::max(size.x.to_pixels(), minimal_size().x), std::max(size.y.to_pixels(), minimal_size().y));
    }

    void Window::on_click(ClickEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        event.handled();

        if (event.action == GLFW_PRESS) {
            to_foreground();
            m_cursor_down_local_pos = core::Input::cursor_pos() - glm::vec2(m_actual_pos.x, m_actual_pos.y);
        }

        glm::ivec2 local_cursor_pos = core::Input::cursor_pos_int() - m_actual_pos;

        if (event.action == GLFW_RELEASE) {
            ungrab_cursor();
            m_dragging = false;
            m_resize_x_left = false;
            m_resize_x_right = false;
            m_resize_y_top = false;
            m_resize_y_bottom = false;
            size = Size::make_pixels(m_actual_size);
            return;
        }

        if (m_close_button->contains(local_cursor_pos)) {
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

    void Window::on_mouse_leave(MouseLeaveEvent&) {
        core::Input::set_cursor(core::Input::CURSOR_DEFAULT);
    }

}
