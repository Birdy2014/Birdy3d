#include "ui/widgets/NumberInput.hpp"

#include "core/Input.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include <iomanip>
#include <sstream>

namespace Birdy3d {

    NumberInput::NumberInput(UIVector position, Placement placement, float val)
        : Widget(position, 0_px, placement) {
        m_value_text = addText(0_px, theme->fontSize, std::to_string(m_value), theme->color_fg, Placement::CENTER_LEFT);
        value(val);
    }

    float NumberInput::value() {
        return m_value;
    }

    void NumberInput::value(float value) {
        m_value = value;
        std::stringstream stream;
        stream << std::fixed << std::setprecision(3) << m_value;
        m_value_text->text = stream.str();
        size = theme->text_renderer()->textSize(m_value_text->text, m_value_text->fontSize);
    }

    bool NumberInput::onScroll(InputScrollEvent* event, bool hover) {
        if (!hover)
            return true;

        float change = event->yoffset;
        if (!Input::keyPressed(GLFW_KEY_LEFT_CONTROL))
            change *= 0.1;
        if (Input::keyPressed(GLFW_KEY_LEFT_SHIFT))
            change *= 0.01;

        value(m_value + change);
        return true;
    }

    bool NumberInput::onClick(InputClickEvent* event, bool hover) {
        return true;
    }

}
