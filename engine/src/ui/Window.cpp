#include "ui/Window.hpp"

#include "core/Input.hpp"
#include "ui/Layout.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d::ui {

    Window::Window(Options options)
        : Container(options) {
        auto line_height = core::Application::theme().line_height();
        m_padding = glm::vec4(BORDER_SIZE, BORDER_SIZE, line_height, BORDER_SIZE);
        add_filled_rectangle(0_px, UIVector(100_p, 100_p - line_height), utils::Color::Name::BG, Placement::BOTTOM_LEFT);
        add_filled_rectangle(0_px, UIVector(100_p, line_height), utils::Color::Name::BG_TITLE_BAR, Placement::TOP_LEFT);
        add_rectangle(0_px, 100_p, utils::Color::Name::BORDER);
        m_close_button = add_filled_rectangle(UIVector(-4_px, 4_px), UIVector(line_height - 8), utils::Color::Name::RED, Placement::TOP_RIGHT);
        m_title = add_text(UIVector(10_px, 3_px), "", utils::Color::Name::FG, Placement::TOP_LEFT);
    }

    void Window::to_foreground() {
        parent->to_foreground(this);
    }

    glm::vec2 Window::minimal_size() {
        glm::vec2 layout_minsize = Widget::minimal_size();
        float min_width = m_title->size().x + m_close_button->size().x + BORDER_SIZE * 2 + 14;
        return glm::vec2(std::max(layout_minsize.x, min_width), layout_minsize.y);
    }

    void Window::on_update() {
        Widget::on_update();

        m_dragged = false;

        if (!is_hovering())
            return;

        glm::vec2 local_cursor_pos = core::Input::cursor_pos() - m_actual_pos;

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
            options.pos = options.pos + core::Input::cursor_pos_offset();
            m_dragged = true;
        }
        glm::vec2 minsize = m_layout->minimal_size(m_children) + glm::vec2(m_padding[0] + m_padding[1], m_padding[2] + m_padding[3]);
        if (m_resize_x_left) {
            float diffold = options.size.x - minsize.x;
            options.size.x -= core::Input::cursor_pos_offset().x;
            float diffnew = options.size.x - minsize.x;
            if (diffold >= 0 && diffnew >= 0) {
                options.pos.x += core::Input::cursor_pos_offset().x;
            } else if (diffold >= 0 && diffnew < 0) {
                options.pos.x += diffold;
            } else if (diffold < 0 && diffnew >= 0) {
                options.pos.x -= diffnew;
            }
        }
        if (m_resize_x_right) {
            options.size.x += core::Input::cursor_pos_offset().x;
        }
        if (m_resize_y_top) {
            float diffold = options.size.y - minsize.y;
            options.size.y -= core::Input::cursor_pos_offset().y;
            float diffnew = options.size.y - minsize.y;
            if (diffold >= 0 && diffnew >= 0) {
                options.pos.y += core::Input::cursor_pos_offset().y;
            } else if (diffold >= 0 && diffnew < 0) {
                options.pos.y += diffold;
            } else if (diffold < 0 && diffnew >= 0) {
                options.pos.y -= diffnew;
            }
        }
        if (m_resize_y_bottom) {
            options.size.y += core::Input::cursor_pos_offset().y;
        }
        m_actual_pos = options.pos;
        m_actual_size = glm::vec2(std::max(options.size.x.to_pixels(), minimal_size().x), std::max(options.size.y.to_pixels(), minimal_size().y));
    }

    bool Window::on_click(const events::InputClickEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT)
            return false;

        if (event.action == GLFW_PRESS)
            to_foreground();

        glm::vec2 local_cursor_pos = core::Input::cursor_pos() - m_actual_pos;

        if (event.action == GLFW_RELEASE) {
            ungrab_cursor();
            m_dragging = false;
            m_resize_x_left = false;
            m_resize_x_right = false;
            m_resize_y_top = false;
            m_resize_y_bottom = false;
            options.size = m_actual_size;
            return false;
        }

        if (m_close_button->contains(local_cursor_pos)) {
            if (callback_close)
                callback_close();
            return false;
        }

        grab_cursor();

        // Moving
        if (m_hover_drag) {
            m_dragging = true;
            return false;
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
        return false;
    }

    void Window::on_mouse_leave() {
        core::Input::set_cursor(core::Input::CURSOR_DEFAULT);
    }

}
