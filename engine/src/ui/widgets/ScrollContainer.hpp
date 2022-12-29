#pragma once

#include "ui/widgets/Scrollable.hpp"

namespace Birdy3d::ui {

    class ScrollContainer : public Scrollable {
    public:
        ScrollContainer(is_widget_options auto options)
            : Scrollable(options)
        { }

        using Scrollable::m_horizontal_scroll_enabled;
        using Scrollable::m_vertical_scroll_enabled;
        using Widget::add_child;
        using Widget::clear_children;
        using Widget::get_widget;
        using Widget::set_layout;
        using Widget::unset_layout;

    protected:
        glm::ivec2 content_size() override
        {
            glm::ivec2 minsize(m_padding.left.to_pixels() + m_padding.right.to_pixels(), m_padding.top.to_pixels() + m_padding.bottom.to_pixels());
            if (m_layout && m_children_visible) {
                if (!m_horizontal_scroll_enabled && !m_vertical_scroll_enabled) {
                    minsize = m_absolute_rect.size();
                }
                if (m_horizontal_scroll_enabled && !m_vertical_scroll_enabled) {
                    minsize.x += m_layout->minimal_size(m_children, m_absolute_rect.height(), Layout::Direction::VERTICAL).x;
                }
                if (!m_horizontal_scroll_enabled && m_vertical_scroll_enabled) {
                    minsize.y += m_layout->minimal_size(m_children, m_absolute_rect.width(), Layout::Direction::HORIZONTAL).y;
                }
                if (m_horizontal_scroll_enabled && m_vertical_scroll_enabled) {
                    minsize += m_layout->minimal_size(m_children);
                }
            }
            return glm::ivec2(std::max(minsize.x, m_absolute_rect.width()), std::max(minsize.y, m_absolute_rect.height()));
        }
    };

}
