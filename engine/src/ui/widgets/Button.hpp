#pragma once

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Widget.hpp"
#include <functional>

namespace Birdy3d::ui {

    class Button : public Widget {
    public:
        struct Options {
            BIRDY3D_WIDGET_OPTIONS_STRUCT
            std::string text;
        };

        Text* button_text;

        Button(is_widget_options auto options)
            : Widget(options)
        {
            add_filled_rectangle(0_px, 100_pc, utils::Color::Name::BG, Placement::BOTTOM_LEFT);
            add_rectangle(0_px, 100_pc, utils::Color::Name::BORDER, Placement::BOTTOM_LEFT);
            button_text = add_text(0_px, options.text, utils::Color::Name::FG, Placement::CENTER);
        };

        glm::ivec2 minimal_size() override
        {
            auto min_size = core::Application::theme().text_renderer().text_size(button_text->text(), button_text->font_size).to_pixels() + glm::ivec2(2);
            return glm::max(min_size, Widget::minimal_size());
        }

        std::function<void()> callback_click;

    protected:
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
