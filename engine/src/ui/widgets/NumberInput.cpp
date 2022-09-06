#include "ui/widgets/NumberInput.hpp"

#include "core/Input.hpp"
#include <iomanip>
#include <sstream>

namespace Birdy3d::ui {

    NumberInput::NumberInput(Options options)
        : TextField(options) {
        value(options.value);
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
                change *= 0.1f;
            if (core::Input::key_pressed(GLFW_KEY_LEFT_SHIFT))
                change *= 0.01f;
            value(value() + change);
        }
        TextField::on_update();
    }

    void NumberInput::on_scroll(ScrollEvent& event) {
        float change = event.yoffset;
        if (!core::Input::key_pressed(GLFW_KEY_LEFT_CONTROL))
            change *= 0.1f;
        if (core::Input::key_pressed(GLFW_KEY_LEFT_SHIFT))
            change *= 0.01f;

        value(m_value + change);
    }

    void NumberInput::on_click(ClickEvent& event) {
        event.handled();

        if (event.button != GLFW_MOUSE_BUTTON_RIGHT)
            TextField::on_click(event);

        if (event.action == GLFW_PRESS) {
            grab_cursor();
            m_dragging = true;
        } else {
            ungrab_cursor();
            m_dragging = false;
        }
    }

    void NumberInput::on_key(KeyEvent& event) {
        TextField::on_key(event);
        set_text_value();
    }

    void NumberInput::on_char(CharEvent& event) {
        if ((event.codepoint < '0' || event.codepoint > '9') && event.codepoint != '.')
            return;
        TextField::on_char(event);
        set_text_value();
    }

    void NumberInput::on_focus_lost(FocusLostEvent& event) {
        TextField::on_focus_lost(event);
        m_dragging = false;
    }

    void NumberInput::set_text_value() {
        auto t = text();
        if (t.empty() || std::count(t.begin(), t.end(), '.') > 1)
            return;
        try {
            m_value = std::clamp(std::stof(t), min_value, max_value);
        } catch (std::invalid_argument) { }
    }

}
