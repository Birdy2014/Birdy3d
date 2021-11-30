#include "ui/widgets/NumberInput.hpp"

#include "core/Input.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include <iomanip>
#include <sstream>

namespace Birdy3d::ui {

    NumberInput::NumberInput(UIVector position, UIVector size, Placement placement, float val)
        : TextField(position, size, placement) {
        value(val);
    }

    float NumberInput::value() {
        return m_value;
    }

    void NumberInput::value(float value) {
        m_value = std::clamp(value, min_value, max_value);
        std::stringstream stream;
        stream << std::fixed << std::setprecision(3) << m_value;
        text(stream.str());
    }

    void NumberInput::on_update() {
        if (m_dragging) {
            glm::vec2 offsets = core::Input::cursor_pos_offset();
            float change = offsets.x + offsets.y;
            if (!core::Input::key_pressed(GLFW_KEY_LEFT_CONTROL))
                change *= 0.1;
            if (core::Input::key_pressed(GLFW_KEY_LEFT_SHIFT))
                change *= 0.01;
            value(value() + change);
        }
        TextField::on_update();
    }

    void NumberInput::on_scroll(const events::InputScrollEvent& event) {
        float change = event.yoffset;
        if (!core::Input::key_pressed(GLFW_KEY_LEFT_CONTROL))
            change *= 0.1;
        if (core::Input::key_pressed(GLFW_KEY_LEFT_SHIFT))
            change *= 0.01;

        value(m_value + change);
    }

    void NumberInput::on_click(const events::InputClickEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_RIGHT)
            return TextField::on_click(event);

        if (event.action == GLFW_PRESS) {
            grab_cursor();
            m_dragging = true;
        } else {
            ungrab_cursor();
            m_dragging = false;
        }
    }

    void NumberInput::on_key(const events::InputKeyEvent& event) {
        TextField::on_key(event);
        value(std::stof(text()));
    }

    void NumberInput::on_char(const events::InputCharEvent& event) {
        if (event.codepoint < '0' || event.codepoint > '9')
            return;
        TextField::on_char(event);
        value(std::stof(text()));
    }

    void NumberInput::on_focus_lost() {
        TextField::on_focus_lost();
        m_dragging = false;
    }

}
