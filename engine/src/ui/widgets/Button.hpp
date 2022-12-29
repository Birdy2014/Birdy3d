#pragma once

#include "core/Input.hpp"
#include "ui/Widget.hpp"
#include <functional>

namespace Birdy3d::ui {

    class Button : public Widget {
    public:
        struct Options {
            BIRDY3D_WIDGET_OPTIONS_STRUCT
            std::string text;
        };

        TextDescription button_text;

        Button(is_widget_options auto options)
            : Widget(options)
        {
            button_text = options.text;
        };

        glm::ivec2 minimal_size() override
        {
            auto min_size = button_text.text_size() + glm::ivec2(2);
            return glm::max(min_size, Widget::minimal_size());
        }

        std::function<void()> callback_click;

    protected:
        void draw() override
        {
            paint_background(true);
            paint_text(0_px, Placement::CENTER, button_text);
        }

        void on_click(ClickEvent& event) override
        {
            if (callback_click && event.action == GLFW_PRESS) {
                event.handled();
                callback_click();
            }
        }

        void on_mouse_enter(MouseEnterEvent&) override
        {
            core::Input::set_cursor(core::Input::CURSOR_HAND);
        }

        void on_mouse_leave(MouseLeaveEvent&) override
        {
            core::Input::set_cursor(core::Input::CURSOR_DEFAULT);
        }
    };

}
