#pragma once

#include "core/Application.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d {

    class Button : public Widget {
    public:
        Button(glm::vec2 pos, Placement placement, std::string text, float fontSize) {
            this->pos = pos;
            this->placement = placement;
            this->unit = Unit::PIXELS;
            this->size = Application::getTextRenderer()->textSize(text, fontSize) + 2.0f;

            Theme* theme = Application::defaultTheme;
            Logger::assertNotNull(theme, "no default theme set");

            addFilledRectangle(glm::vec2(0), glm::vec2(1), theme->color_bg, Placement::BOTTOM_LEFT, Unit::PERCENT);
            addRectangle(glm::vec2(0), glm::vec2(1), theme->color_border, Placement::BOTTOM_LEFT, Unit::PERCENT);
            addText(glm::vec2(0), fontSize, text, theme->color_fg, Placement::CENTER);
        };
    };

}
