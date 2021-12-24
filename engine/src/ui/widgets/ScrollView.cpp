#include "ui/widgets/ScrollView.hpp"

#include "core/Input.hpp"
#include "ui/Rectangle.hpp"

namespace Birdy3d::ui {

    ScrollView::ScrollView(Options options)
        : Widget(options) {
        m_scrollbar_background = add_filled_rectangle(0_px, UIVector(10_px, 100_p), utils::Color::Name::BG, Placement::TOP_RIGHT);
        m_scrollbar_bar = add_filled_rectangle(0_px, UIVector(10_px, 100_p), utils::Color::Name::FG, Placement::TOP_RIGHT);
        m_padding = glm::vec4(0, 10, 0, 0);
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
        m_max_scroll_offset = m_content_size - m_actual_size;
        m_max_scroll_offset.x = std::max(m_max_scroll_offset.x, 0.0f);
        m_max_scroll_offset.y = std::max(m_max_scroll_offset.y, 0.0f);

        float scrollbar_height_percentage = (m_actual_size.y / m_content_size.y) * 100;
        float scrollbar_y_offset_percentage = (m_scroll_offset.y / m_max_scroll_offset.y) * 100;
        scrollbar_y_offset_percentage = (scrollbar_y_offset_percentage / 100) * (100 - scrollbar_height_percentage);
        m_scrollbar_bar->size(UIVector(m_scrollbar_bar->size().x, Unit(0, scrollbar_height_percentage)));
        m_scrollbar_bar->position(UIVector(m_scrollbar_bar->position().x, Unit(0, -scrollbar_y_offset_percentage)));

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
        m_scroll_offset.x -= event.xoffset * acceleration;
        m_scroll_offset.y -= event.yoffset * acceleration;

        check_scroll_bounds();
    }

    // FIXME: on_click is not called if the ScrollView is also scrolled horizontally
    void ScrollView::on_click(const events::InputClickEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT)
            return;
        if (event.action == GLFW_PRESS && m_scrollbar_bar->contains(core::Input::cursor_pos() - m_actual_pos)) {
            m_scrollbar_grabbed = true;
            grab_cursor();
        } else if (m_scrollbar_grabbed) {
            m_scrollbar_grabbed = false;
            ungrab_cursor();
        }
    }

    void ScrollView::on_update() {
        if (m_scrollbar_grabbed) {
            m_scroll_offset.y -= core::Input::cursor_pos_offset().y * (m_content_size.y / m_actual_size.y);
            check_scroll_bounds();
        }
        Widget::on_update();
    }

    void ScrollView::check_scroll_bounds() {
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
