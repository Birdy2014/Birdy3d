#pragma once

#include "ui/Container.hpp"
#include "ui/widgets/Scrollable.hpp"

namespace Birdy3d::ui {

    class ScrollContainer : public Scrollable {
    public:
        ScrollContainer(Options options)
            : Scrollable(options) { }

        using Scrollable::m_horizontal_scroll_enabled;
        using Scrollable::m_vertical_scroll_enabled;
        using Widget::add_child;
        using Widget::clear_children;
        using Widget::get_widget;
        using Widget::set_layout;
        using Widget::unset_layout;
    };

}
