#pragma once

#include "core/Application.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Widget.hpp"

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

            Application::eventBus->subscribe(this, &Button::onClick);
        };

        glm::vec2 minimalSize() override {
            glm::vec2 minSize = Application::getTextRenderer()->textSize(texts[0]->text, texts[0]->fontSize) + 2.0f;
            return glm::max(minSize, Widget::minimalSize());
        }

        void (*clickCallback)(InputClickEvent*) = nullptr;

    private:
        void onClick(InputClickEvent* event) {
            if (hover && clickCallback && event->action == GLFW_PRESS)
                clickCallback(event);
        }
    };

}
