#ifndef BIRDY3D_BUTTON_HPP
#define BIRDY3D_BUTTON_HPP

#include "core/Application.hpp"
#include "ui/Widget.hpp"

class Button : public Widget {
public:
    Button(glm::vec2 pos, Placement placement, std::string text, float fontSize, glm::vec4 color, glm::vec4 textColor, glm::vec4 borderColor) {
        this->pos = pos;
        this->placement = placement;
        this->unit = Unit::PIXELS;
        this->size = Application::getTextRenderer()->textSize(text, fontSize) + 2.0f;

        addFilledRectangle(glm::vec2(0), glm::vec2(1), color, Placement::BOTTOM_LEFT, Unit::PERCENT);
        addRectangle(glm::vec2(0), glm::vec2(1), borderColor, Placement::BOTTOM_LEFT, Unit::PERCENT);
        addText(glm::vec2(0), fontSize, text, textColor, Placement::CENTER);
    };
};

#endif
