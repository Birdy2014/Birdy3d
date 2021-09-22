#pragma once

#include "core/Application.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Widget.hpp"
#include <functional>

namespace Birdy3d {

    class Button : public Widget {
    public:
        Text* button_text;

        Button(UIVector pos, Placement placement, std::string text, UIVector size = UIVector(0))
            : Widget(pos, size, placement) {
            Logger::assertNotNull(Application::theme, "no default theme set");

            add_filled_rectangle(UIVector(0_px), UIVector(100_p), Application::theme->color_bg, Placement::BOTTOM_LEFT);
            add_rectangle(UIVector(0_px), UIVector(100_p), Application::theme->color_border, Placement::BOTTOM_LEFT);
            button_text = add_text(UIVector(0_px), Application::theme->font_size, text, Application::theme->color_fg, Placement::CENTER);
        };

        glm::vec2 minimalSize() override {
            glm::vec2 minSize = Application::theme->text_renderer()->textSize(button_text->text(), button_text->fontSize) + 2.0f;
            return glm::max(minSize, Widget::minimalSize());
        }

        std::function<void(InputClickEvent*)> callback_click;

    protected:
        void on_click(InputClickEvent* event) override {
            if (callback_click && event->action == GLFW_PRESS) {
                callback_click(event);
            }
        }

        void on_mouse_enter() override {
            Input::setCursor(Input::CURSOR_HAND);
        }

        void on_mouse_leave() override {
            Input::setCursor(Input::CURSOR_DEFAULT);
        }
    };

}
