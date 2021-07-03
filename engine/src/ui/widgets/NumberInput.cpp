#include "ui/widgets/NumberInput.hpp"

#include "core/Input.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include <iomanip>
#include <sstream>

namespace Birdy3d {

    NumberInput::NumberInput(UIVector position, UIVector size, Placement placement, float val)
        : TextField(position, size, placement) {
        value(val);
    }

    float NumberInput::value() {
        return m_value;
    }

    void NumberInput::value(float value) {
        m_value = value;
        std::stringstream stream;
        stream << std::fixed << std::setprecision(3) << m_value;
        text(stream.str());
    }

    bool NumberInput::update(bool hover) {
        if (m_dragging) {
            glm::vec2 offsets = Input::cursorPosOffset();
            float change = offsets.x + offsets.y;
            if (!Input::keyPressed(GLFW_KEY_LEFT_CONTROL))
                change *= 0.1;
            if (Input::keyPressed(GLFW_KEY_LEFT_SHIFT))
                change *= 0.01;
            value(value() + change);
        }
        return TextField::update(hover);
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
        if (event->button != GLFW_MOUSE_BUTTON_RIGHT)
            return TextField::onClick(event, hover);

        if (hover && event->action == GLFW_PRESS)
            m_dragging = true;

        if (event->action == GLFW_RELEASE)
            m_dragging = false;

        return true;
    }

    bool NumberInput::onKey(InputKeyEvent* event, bool hover) {
        TextField::onKey(event, hover);
        value(std::stof(text()));
        return true;
    }

    bool NumberInput::onChar(InputCharEvent* event, bool hover) {
        if (event->codepoint < '0' || event->codepoint > '9')
            return true;
        TextField::onChar(event, hover);
        value(std::stof(text()));
        return true;
    }

}
