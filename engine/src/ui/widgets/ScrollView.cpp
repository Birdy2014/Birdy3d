#include "ui/widgets/ScrollView.hpp"

namespace Birdy3d {

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

        // NOTE: This can probably be removed once the y coordinate is flipped
        glm::vec2 pos_offset = glm::vec2(0, std::min(size.y - minsize.y, 0.0f));

        for (const auto& s : m_shapes) {
            s->parent_size(m_content_size);
        }

        if (m_layout && m_children_visible)
            m_layout->arrange(m_children, pos + glm::vec2(m_padding[0], m_padding[2]) + m_scroll_offset + pos_offset, m_content_size - glm::vec2(m_padding[0] + m_padding[1], m_padding[2] + m_padding[3]));

        if (resized)
            on_resize();
    }

    void ScrollView::on_scroll(const InputScrollEvent& event) {
        int acceleration = 10;
        m_scroll_offset.x -= event.xoffset * acceleration;
        m_scroll_offset.y -= event.yoffset * acceleration;

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
