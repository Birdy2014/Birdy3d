#include "ui/widgets/Scrollable.hpp"

#include "core/Input.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d::ui {

    glm::ivec2 Scrollable::minimal_size()
    {
        if (!m_horizontal_scroll_enabled && !m_vertical_scroll_enabled)
            return Widget::minimal_size();
        if (m_horizontal_scroll_enabled && !m_vertical_scroll_enabled) {
            auto minsize_horizontal = m_padding.left.to_pixels() + m_padding.right.to_pixels();
            if (m_layout && m_children_visible)
                minsize_horizontal += m_layout->minimal_size(m_children, Layout::Direction::VERTICAL);
            return {minsize_horizontal, 1};
        }
        if (!m_horizontal_scroll_enabled && m_vertical_scroll_enabled) {
            auto minsize_vertical = m_padding.top.to_pixels() + m_padding.bottom.to_pixels();
            if (m_layout && m_children_visible)
                minsize_vertical += m_layout->minimal_size(m_children, Layout::Direction::HORIZONTAL);
            return {1, minsize_vertical};
        }
        return glm::ivec2(1);
    }

    void Scrollable::do_layout(Rect const& rect)
    {
        auto content_size_cache = content_size();

        bool resized = rect.size() != m_absolute_rect.size();
        m_absolute_rect = rect;

        if (m_layout && m_children_visible) {
            auto padding_top_left = glm::ivec2(m_padding.left.to_pixels(), m_padding.top.to_pixels());
            auto padding_size = glm::ivec2(m_padding.left.to_pixels() + m_padding.right.to_pixels(), m_padding.top.to_pixels() + m_padding.bottom.to_pixels());
            m_layout->arrange(m_children, m_absolute_rect.position() - m_scroll_offset + padding_top_left, content_size_cache - (m_max_scroll_offset - m_scroll_offset) - padding_size);
        }

        if (resized) {
            auto event = ResizeEvent{};
            notify_event(event);
        }
    }

    void Scrollable::draw()
    {
        auto fg_color = core::Application::theme().color(utils::Color::Name::FG);
        if (m_scrollbar_vertical_visible)
            paint_rectangle_filled(m_scrollbar_vertical_rect, fg_color);
        if (m_scrollbar_horizontal_visible)
            paint_rectangle_filled(m_scrollbar_horizontal_rect, fg_color);
    }

    void Scrollable::on_scroll(ScrollEvent& event)
    {
        event.handled();

        float speed = 10.0f;
        m_scroll_offset.x -= event.xoffset * speed;
        m_scroll_offset.y -= event.yoffset * speed;

        check_scroll_bounds();
    }

    void Scrollable::on_click(ClickEvent& event)
    {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        event.handled();

        if (event.action == GLFW_PRESS) {
            auto local_cursor_pos = core::Input::cursor_pos_int() - m_absolute_rect.position();
            if (m_scrollbar_vertical_rect.contains(local_cursor_pos)) {
                m_scrollbar_vertical_grabbed = true;
                grab_cursor();
                return;
            }
            if (m_scrollbar_horizontal_rect.contains(local_cursor_pos)) {
                m_scrollbar_horizontal_grabbed = true;
                grab_cursor();
                return;
            }
        }

        m_scrollbar_horizontal_grabbed = false;
        m_scrollbar_vertical_grabbed = false;
        ungrab_cursor();
    }

    void Scrollable::on_update()
    {
        auto content_size_cache = content_size();

        m_max_scroll_offset.x = std::max(content_size_cache.x - m_absolute_rect.width(), 0);
        m_max_scroll_offset.y = std::max(content_size_cache.y - m_absolute_rect.height(), 0);

        // Vertical scollbar
        if (m_vertical_scroll_enabled && m_max_scroll_offset.y > 0) {
            m_scrollbar_vertical_visible = true;

            m_scrollbar_vertical_rect.width(10);
            m_scrollbar_vertical_rect.left(m_absolute_rect.width() - 10);

            auto scrollbar_height_ratio = static_cast<float>(m_absolute_rect.height()) / static_cast<float>(content_size_cache.y);
            auto scrollbar_y_offset_ratio = 0.0f;
            if (m_max_scroll_offset.y > 0.0f)
                scrollbar_y_offset_ratio = (static_cast<float>(m_scroll_offset.y) / static_cast<float>(m_max_scroll_offset.y)) * (1.0f - scrollbar_height_ratio);

            m_scrollbar_vertical_rect.height(scrollbar_height_ratio * m_absolute_rect.height());
            m_scrollbar_vertical_rect.top(scrollbar_y_offset_ratio * m_absolute_rect.height());
        } else {
            m_scrollbar_vertical_visible = false;
        }

        // Horizontal scollbar
        if (m_horizontal_scroll_enabled && m_max_scroll_offset.x > 0) {
            m_scrollbar_horizontal_visible = true;

            m_scrollbar_horizontal_rect.height(10);
            m_scrollbar_horizontal_rect.top(m_absolute_rect.height() - 10);

            auto scrollbar_width_ratio = static_cast<float>(m_absolute_rect.width()) / static_cast<float>(content_size_cache.x);
            auto scrollbar_x_offset_ratio = 0.0f;
            if (m_max_scroll_offset.x > 0.0f)
                scrollbar_x_offset_ratio = (static_cast<float>(m_scroll_offset.x) / static_cast<float>(m_max_scroll_offset.x)) * (1.0f - scrollbar_width_ratio);

            m_scrollbar_horizontal_rect.width(scrollbar_width_ratio * m_absolute_rect.width());
            m_scrollbar_horizontal_rect.left(scrollbar_x_offset_ratio * m_absolute_rect.width());
        } else {
            m_scrollbar_horizontal_visible = false;
        }

        // Scrolling
        if (m_scrollbar_vertical_grabbed) {
            m_scroll_offset.y += core::Input::cursor_pos_offset().y * (content_size_cache.y / m_absolute_rect.height());
            check_scroll_bounds();
        }
        if (m_scrollbar_horizontal_grabbed) {
            m_scroll_offset.x += core::Input::cursor_pos_offset().x * (content_size_cache.x / m_absolute_rect.width());
            check_scroll_bounds();
        }
        Widget::on_update();
    }

    void Scrollable::on_resize(ResizeEvent&)
    {
        check_scroll_bounds();
    }

    void Scrollable::check_scroll_bounds()
    {
        if (m_scroll_offset.x < 0)
            m_scroll_offset.x = 0;
        if (m_scroll_offset.x > m_max_scroll_offset.x)
            m_scroll_offset.x = m_max_scroll_offset.x;
        if (m_scroll_offset.y < 0)
            m_scroll_offset.y = 0;
        if (m_scroll_offset.y > m_max_scroll_offset.y)
            m_scroll_offset.y = m_max_scroll_offset.y;
    }
}
