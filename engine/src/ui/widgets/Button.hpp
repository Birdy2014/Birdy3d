#pragma once

#include "core/Application.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Widget.hpp"
#include <functional>

namespace Birdy3d {

    class Button : public Widget {
    public:
        Button(UIVector pos, Placement placement, std::string text, float fontSize, UIVector size = UIVector(0)) {
            this->pos = pos;
            this->placement = placement;
            this->size = size;

            Logger::assertNotNull(theme, "no default theme set");

            addFilledRectangle(UIVector(0_px), UIVector(100_p), theme->color_bg, Placement::BOTTOM_LEFT);
            addRectangle(UIVector(0_px), UIVector(100_p), theme->color_border, Placement::BOTTOM_LEFT);
            addText(UIVector(0_px), fontSize, text, theme->color_fg, Placement::CENTER);
        };

        glm::vec2 minimalSize() override {
            glm::vec2 minSize = theme->text_renderer()->textSize(getShape<Text>()->text, getShape<Text>()->fontSize) + 2.0f;
            return glm::max(minSize, Widget::minimalSize());
        }

        std::function<void(InputClickEvent*)> clickCallback;

    protected:
        bool onClick(InputClickEvent* event, bool hover) override {
            if (hover && clickCallback && event->action == GLFW_PRESS) {
                clickCallback(event);
                return true;
            }
            return true;
        }

        void onMouseEnter() override {
            Input::setCursor(Input::CURSOR_HAND);
        }

        void onMouseLeave() override {
            Input::setCursor(Input::CURSOR_DEFAULT);
        }
    };

}
