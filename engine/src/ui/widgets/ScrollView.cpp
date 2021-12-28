#include "ui/widgets/ScrollView.hpp"

#include "core/Input.hpp"
#include "ui/Rectangle.hpp"

namespace Birdy3d::ui {

    ScrollView::ScrollView(Options options)
        : Widget(options) {
        m_scrollbar_bar_vertical = add_filled_rectangle(0_px, UIVector(10_px, 100_p), utils::Color::Name::FG, Placement::TOP_RIGHT);
        m_scrollbar_bar_vertical->in_foreground = true;
        m_scrollbar_bar_horizontal = add_filled_rectangle(0_px, UIVector(100_p, 10_px), utils::Color::Name::FG, Placement::BOTTOM_LEFT);
        m_scrollbar_bar_horizontal->in_foreground = true;
        m_padding = glm::vec4(0, 10, 10, 0);
    }

    glm::vec2 ScrollView::minimal_size() {
        return glm::vec2(1);
    }

    void ScrollView::arrange(glm::vec2 pos, glm::vec2 size) {
        bool resized = false;
        if (size != m_actual_size)
            resized = true;
        m_actual_pos = pos;
        m_actual_size = size;
        m_move = glm::translate(glm::mat4(1), glm::vec3(m_actual_pos, 0.0f));

        glm::vec2 minsize(m_padding[0] + m_padding[1], m_padding[2] + m_padding[3]);
        if (m_layout && m_children_visible)
            minsize += m_layout->minimal_size(m_children);
        m_content_size = glm::vec2(glm::max(minsize.x, size.x), glm::max(minsize.y, size.y));
        m_max_scroll_offset.x = std::min(m_actual_size.x - m_content_size.x, 0.0f);
        m_max_scroll_offset.y = std::max(m_content_size.y - m_actual_size.y, 0.0f);

        // Vertical scollbar
        float scrollbar_height_percentage = (m_actual_size.y / m_content_size.y) * 100;
        float scrollbar_y_offset_percentage = (m_scroll_offset.y / m_max_scroll_offset.y) * 100;
        scrollbar_y_offset_percentage = (scrollbar_y_offset_percentage / 100) * (100 - scrollbar_height_percentage);
        m_scrollbar_bar_vertical->size(UIVector(m_scrollbar_bar_vertical->size().x, Unit(0, scrollbar_height_percentage)));
        m_scrollbar_bar_vertical->position(UIVector(m_scrollbar_bar_vertical->position().x, Unit(0, -scrollbar_y_offset_percentage)));

        // Horizontal scollbar
        float scrollbar_width_percentage = (m_actual_size.x / m_content_size.x) * 100;
        float scrollbar_x_offset_percentage = (m_scroll_offset.x / m_max_scroll_offset.x) * 100;
        scrollbar_x_offset_percentage = (scrollbar_x_offset_percentage / 100) * (100 - scrollbar_width_percentage);
        m_scrollbar_bar_horizontal->size(UIVector(Unit(0, scrollbar_width_percentage), m_scrollbar_bar_horizontal->size().y));
        m_scrollbar_bar_horizontal->position(UIVector(Unit(0, scrollbar_x_offset_percentage), m_scrollbar_bar_horizontal->position().y));

        // NOTE: This can probably be removed once the y coordinate is flipped
        glm::vec2 pos_offset = glm::vec2(0, std::min(size.y - minsize.y, 0.0f));

        for (const auto& s : m_shapes) {
            s->parent_size(m_actual_size);
        }

        if (m_layout && m_children_visible)
            m_layout->arrange(m_children, pos + glm::vec2(m_padding[0], m_padding[2]) + m_scroll_offset + pos_offset, m_content_size - glm::vec2(m_padding[0] + m_padding[1], m_padding[2] + m_padding[3]));

        if (resized)
            on_resize();
    }

    void ScrollView::on_scroll(const events::InputScrollEvent& event) {
        int acceleration = 10;
        m_scroll_offset.x += event.xoffset * acceleration;
        m_scroll_offset.y -= event.yoffset * acceleration;

        check_scroll_bounds();
    }

    void ScrollView::on_click(const events::InputClickEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        if (event.action == GLFW_PRESS) {
            if (m_scrollbar_bar_vertical->contains(core::Input::cursor_pos() - m_actual_pos)) {
                m_scrollbar_vertical_grabbed = true;
                grab_cursor();
                return;
            }
            if (m_scrollbar_bar_horizontal->contains(core::Input::cursor_pos() - m_actual_pos)) {
                m_scrollbar_horizontal_grabbed = true;
                grab_cursor();
                return;
            }
        }

        m_scrollbar_horizontal_grabbed = false;
        m_scrollbar_vertical_grabbed = false;
        ungrab_cursor();
    }

    void ScrollView::on_update() {
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

    void ScrollView::check_scroll_bounds() {
        if (m_scroll_offset.x > 0)
            m_scroll_offset.x = 0;
        if (m_scroll_offset.x < m_max_scroll_offset.x)
            m_scroll_offset.x = m_max_scroll_offset.x;
        if (m_scroll_offset.y < 0)
            m_scroll_offset.y = 0;
        if (m_scroll_offset.y > m_max_scroll_offset.y)
            m_scroll_offset.y = m_max_scroll_offset.y;
    }
}
