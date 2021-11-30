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
        Text* button_text;

        Button(UIVector pos, Placement placement, std::string text, UIVector size = UIVector(0))
            : Widget(pos, size, placement) {
            add_filled_rectangle(UIVector(0_px), UIVector(100_p), utils::Color::Name::BG, Placement::BOTTOM_LEFT);
            add_rectangle(UIVector(0_px), UIVector(100_p), utils::Color::Name::BORDER, Placement::BOTTOM_LEFT);
            button_text = add_text(UIVector(0_px), text, utils::Color::Name::FG, Placement::CENTER);
        };

        glm::vec2 minimal_size() override {
            glm::vec2 min_size = core::Application::theme().text_renderer().text_size(button_text->text(), button_text->font_size) + 2.0f;
            return glm::max(min_size, Widget::minimal_size());
        }

        std::function<void(const events::InputClickEvent&)> callback_click;

    protected:
        void on_click(const events::InputClickEvent& event) override {
            if (callback_click && event.action == GLFW_PRESS) {
                callback_click(event);
            }
        }

        void on_mouse_enter() override {
            core::Input::set_cursor(core::Input::CURSOR_HAND);
        }

        void on_mouse_leave() override {
            core::Input::set_cursor(core::Input::CURSOR_DEFAULT);
        }
    };

}
