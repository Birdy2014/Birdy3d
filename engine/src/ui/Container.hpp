#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    /**
     * @brief Widget that can contain other widgets.
     *
     * This widget makes the functions for changing layout and child widgets public.
     * It can be used to create more complex layouts or overridden to create different containers.
     */
    class Container : public Widget {
    public:
        using Widget::Widget;

        using Widget::add_child;
        using Widget::clear_children;
        using Widget::get_widget;
        using Widget::set_layout;
        using Widget::unset_layout;
    };

}
