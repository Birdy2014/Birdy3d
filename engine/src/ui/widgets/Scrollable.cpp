#include "ui/widgets/Scrollable.hpp"

#include "core/Input.hpp"

namespace Birdy3d::ui {

    glm::ivec2 Scrollable::minimal_size() {
        if (!m_horizontal_scroll_enabled && !m_vertical_scroll_enabled)
            return Widget::minimal_size();
        if (m_horizontal_scroll_enabled && !m_vertical_scroll_enabled) {
            auto minsize_horizontal = m_padding.left.to_pixels() + m_padding.right.to_pixels();
            if (m_layout && m_children_visible)
                minsize_horizontal += m_layout->minimal_size(m_children, Layout::Direction::VERTICAL);
            return { minsize_horizontal, 1 };
        }
        if (!m_horizontal_scroll_enabled && m_vertical_scroll_enabled) {
            auto minsize_vertical = m_padding.top.to_pixels() + m_padding.bottom.to_pixels();
            if (m_layout && m_children_visible)
                minsize_vertical += m_layout->minimal_size(m_children, Layout::Direction::HORIZONTAL);
            return { 1, minsize_vertical };
        }
        return glm::ivec2(1);
    }

    void Scrollable::arrange(glm::ivec2 pos, glm::ivec2 size) {
        bool resized = false;
        if (size != m_actual_size)
            resized = true;
        m_actual_pos = pos;
        m_actual_size = size;

        glm::ivec2 minsize(m_padding.left.to_pixels() + m_padding.right.to_pixels(), m_padding.top.to_pixels() + m_padding.bottom.to_pixels());
        if (m_layout && m_children_visible) {
            if (!m_horizontal_scroll_enabled && !m_vertical_scroll_enabled) {
                minsize = m_actual_size;
            }
            if (m_horizontal_scroll_enabled && !m_vertical_scroll_enabled) {
                minsize.x += m_layout->minimal_size(m_children, m_actual_size.y, Layout::Direction::VERTICAL).x;
            }
            if (!m_horizontal_scroll_enabled && m_vertical_scroll_enabled) {
                minsize.y += m_layout->minimal_size(m_children, m_actual_size.x, Layout::Direction::HORIZONTAL).y;
            }
            if (m_horizontal_scroll_enabled && m_vertical_scroll_enabled) {
                minsize += m_layout->minimal_size(m_children);
            }
        }
        m_content_size = glm::ivec2(glm::max(minsize.x, size.x), glm::max(minsize.y, size.y));

        m_scrollbar_vertical->parent_size(m_actual_size);
        m_scrollbar_horizontal->parent_size(m_actual_size);

        for (const auto& shape : m_shapes) {
            auto shape_pos = shape->position().to_pixels(m_actual_size);
            auto shape_size = shape->size().to_pixels(m_actual_size);
            if (m_content_size.x < shape_pos.x + shape_size.x)
                m_content_size.x = shape_pos.x + shape_size.x;
            if (m_content_size.y < shape_pos.y + shape_size.y)
                m_content_size.y = shape_pos.y + shape_size.y;
        }

        for (const auto& shape : m_shapes) {
            if (shape.get() == m_scrollbar_horizontal || shape.get() == m_scrollbar_vertical)
                shape->parent_size(m_actual_size);
            else
                shape->parent_size(m_content_size);
        }

        m_max_scroll_offset.x = std::min(m_actual_size.x - m_content_size.x, 0);
        m_max_scroll_offset.y = std::min(m_actual_size.y - m_content_size.y, 0);

        // HACK: The content (widgets and shapes) should be scrolled but the scrollbars must stay fixed, so this sets the move matrix for the content
        m_move = glm::translate(glm::mat4(1), glm::vec3(m_actual_pos, 0.0f));
        m_move = glm::translate(m_move, glm::vec3(m_scroll_offset, 0.0f));

        // Vertical scollbar
        m_scrollbar_vertical->hidden(!m_vertical_scroll_enabled);
        if (m_vertical_scroll_enabled) {
            auto scrollbar_height_percentage = (static_cast<float>(m_actual_size.y) / static_cast<float>(m_content_size.y)) * 100.0f;
            auto scrollbar_y_offset_percentage = (static_cast<float>(m_scroll_offset.y) / static_cast<float>(m_max_scroll_offset.y)) * 100.0f;
            scrollbar_y_offset_percentage = (scrollbar_y_offset_percentage / 100.0f) * (100.0f - scrollbar_height_percentage);
            m_scrollbar_vertical->size(Size(m_scrollbar_vertical->size().x, Dimension::make_percent(scrollbar_height_percentage)));
            m_scrollbar_vertical->position(Position(0_px, Dimension::make_percent(scrollbar_y_offset_percentage)));
        }

        // Horizontal scollbar
        m_scrollbar_horizontal->hidden(!m_horizontal_scroll_enabled);
        if (m_horizontal_scroll_enabled) {
            auto scrollbar_width_percentage = (static_cast<float>(m_actual_size.x) / static_cast<float>(m_content_size.x)) * 100.0f;
            auto scrollbar_x_offset_percentage = (static_cast<float>(m_scroll_offset.x) / static_cast<float>(m_max_scroll_offset.x)) * 100.0f;
            scrollbar_x_offset_percentage = (scrollbar_x_offset_percentage / 100) * (100 - scrollbar_width_percentage);
            m_scrollbar_horizontal->size(Size(Dimension::make_percent(scrollbar_width_percentage), m_scrollbar_horizontal->size().y));
            m_scrollbar_horizontal->position(Position(Dimension::make_percent(scrollbar_x_offset_percentage), m_scrollbar_horizontal->position().y));
        }

        if (m_layout && m_children_visible)
            m_layout->arrange(m_children, pos + glm::ivec2(m_padding.left.to_pixels(), m_padding.top.to_pixels()) + m_scroll_offset, m_content_size - glm::ivec2(m_padding.left.to_pixels() + m_padding.right.to_pixels(), m_padding.top.to_pixels() + m_padding.bottom.to_pixels()));

        if (resized) {
            auto resize_event = ResizeEvent {};
            notify_event(resize_event);
        }
    }

    void Scrollable::draw() {
        // HACK: Because the m_move matrix scrolls the content, this ugly code draws the scrollbars with the correct matrix
        auto move = glm::translate(glm::mat4(1), glm::vec3(m_actual_pos, 0.0f));
        m_scrollbar_vertical->color(utils::Color::Name::FG);
        m_scrollbar_horizontal->color(utils::Color::Name::FG);
        m_scrollbar_vertical->draw(move);
        m_scrollbar_horizontal->draw(move);
        m_scrollbar_vertical->color(utils::Color::Name::NONE);
        m_scrollbar_horizontal->color(utils::Color::Name::NONE);
    }

    void Scrollable::on_scroll(ScrollEvent& event) {
        event.handled();

        float speed = 10.0f;
        m_scroll_offset.x += event.xoffset * speed;
        m_scroll_offset.y += event.yoffset * speed;

        check_scroll_bounds();
    }

    void Scrollable::on_click(ClickEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        event.handled();

        if (event.action == GLFW_PRESS) {
            if (m_scrollbar_vertical->contains(core::Input::cursor_pos_int() - m_actual_pos)) {
                m_scrollbar_vertical_grabbed = true;
                grab_cursor();
                return;
            }
            if (m_scrollbar_horizontal->contains(core::Input::cursor_pos_int() - m_actual_pos)) {
                m_scrollbar_horizontal_grabbed = true;
                grab_cursor();
                return;
            }
        }

        m_scrollbar_horizontal_grabbed = false;
        m_scrollbar_vertical_grabbed = false;
        ungrab_cursor();
    }

    void Scrollable::on_update() {
        if (m_scrollbar_vertical_grabbed) {
            m_scroll_offset.y -= core::Input::cursor_pos_offset().y * (m_content_size.y / m_actual_size.y);
            check_scroll_bounds();
        }
        if (m_scrollbar_horizontal_grabbed) {
            m_scroll_offset.x -= core::Input::cursor_pos_offset().x * (m_content_size.x / m_actual_size.x);
            check_scroll_bounds();
        }
        Widget::on_update();
    }

    void Scrollable::on_resize(ResizeEvent&) {
        check_scroll_bounds();
    }

    void Scrollable::check_scroll_bounds() {
        if (m_scroll_offset.x > 0)
            m_scroll_offset.x = 0;
        if (m_scroll_offset.x < m_max_scroll_offset.x)
            m_scroll_offset.x = m_max_scroll_offset.x;
        if (m_scroll_offset.y > 0)
            m_scroll_offset.y = 0;
        if (m_scroll_offset.y < m_max_scroll_offset.y)
            m_scroll_offset.y = m_max_scroll_offset.y;
    }
}
