#include "ui/widgets/TextField.hpp"

#include "core/Input.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "utils/Unicode.hpp"

namespace Birdy3d::ui {

    TextField::TextField(UIVector position, UIVector size, Placement placement)
        : Widget(position, size, placement) {
        add_filled_rectangle(0_px, 100_p, utils::Color::Name::BG_INPUT);
    }

    std::string TextField::text() {
        return utils::Unicode::utf32_to_utf8(m_text);
    }

    void TextField::text(std::string text) {
        std::u32string new_text = utils::Unicode::utf8_to_utf32(text);
        if (new_text != m_text)
            m_changed = true;
        m_text = new_text;
    }

    void TextField::append(std::string text) {
        m_changed = true;
        m_text += utils::Unicode::utf8_to_utf32(text);
    }

    void TextField::clear() {
        m_text.clear();
        m_cursor_pos = m_cursor_pos < 0 ? -1 : 0;
        m_selecting = false;
        m_selection_start = -1;
        m_selection_end = -1;
        m_changed = true;
    }

    void TextField::draw() {
        Widget::draw();
        core::Application::theme().text_renderer().render_text(m_text, m_side_padding, m_actual_size.y / 2 - core::Application::theme().font_size() / 2, core::Application::theme().font_size(), utils::Color::Name::FG, m_move, m_cursor_pos >= 0, m_cursor_pos, m_selection_start != -1 && m_selection_end != -1, m_selection_start, m_selection_end, utils::Color::Name::TEXT_HIGHLIGHT);
    }

    void TextField::on_update() {
        if (m_selecting) {
            glm::vec2 local_pos = core::Input::cursor_pos() - m_actual_pos;
            int char_pos = core::Application::theme().text_renderer().char_index(m_text, core::Application::theme().font_size(), local_pos.x - m_side_padding, true);
            if (m_selection_start == char_pos)
                m_selection_end = -1;
            else
                m_selection_end = char_pos + (m_selection_start < char_pos ? -1 : 0);
        }
    }

    void TextField::on_click(const events::InputClickEvent& event) {
        if (readonly || event.button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        if (event.action == GLFW_PRESS) {
            grab_cursor();
            glm::vec2 local_pos = core::Input::cursor_pos() - m_actual_pos;
            int char_pos = core::Application::theme().text_renderer().char_index(m_text, core::Application::theme().font_size(), local_pos.x - m_side_padding, true);
            m_selecting = true;
            m_selection_start = char_pos;
            m_cursor_pos = -1;
        } else if (event.action == GLFW_RELEASE && m_selection_end == -1) {
            ungrab_cursor();
            m_selecting = false;
            m_cursor_pos = m_selection_start;
            m_selection_start = -1;
            m_selection_end = -1;
        } else if (event.action == GLFW_RELEASE) {
            ungrab_cursor();
            m_selecting = false;
        }
    }

    void TextField::on_key(const events::InputKeyEvent& event) {
        if (readonly || event.action != GLFW_PRESS)
            return;

        if (event.key == GLFW_KEY_ENTER && has_callbacks("accept"))
            return execute_callbacks("accept");

        if (m_selection_start >= 0 && m_selection_end >= 0) {
            if (event.key == GLFW_KEY_DELETE || event.key == GLFW_KEY_BACKSPACE)
                clear_selection();
            return;
        }

        if (m_cursor_pos >= 0) {
            switch (event.key) {
            case GLFW_KEY_DELETE:
                if (m_cursor_pos == (int)m_text.length())
                    break;
                m_text.erase(m_text.begin() + m_cursor_pos, m_text.begin() + m_cursor_pos + 1);
                break;
            case GLFW_KEY_BACKSPACE:
                if (m_cursor_pos == 0)
                    break;
                m_text.erase(m_text.begin() + m_cursor_pos - 1, m_text.begin() + m_cursor_pos);
                m_cursor_pos--;
                break;
            case GLFW_KEY_LEFT:
                if (m_cursor_pos == 0)
                    break;
                m_cursor_pos--;
                break;
            case GLFW_KEY_RIGHT:
                if (m_cursor_pos == (int)m_text.length())
                    break;
                m_cursor_pos++;
                break;
            }
        }
    }

    void TextField::on_char(const events::InputCharEvent& event) {
        if (readonly)
            return;

        clear_selection();

        if (m_cursor_pos < 0 || m_cursor_pos > (int)m_text.size())
            return;

        char32_t c[2];
        c[0] = event.codepoint;
        c[1] = 0;
        m_text.insert(m_cursor_pos, c);
        m_cursor_pos++;
    }

    void TextField::on_mouse_enter() {
        if (!readonly)
            core::Input::set_cursor(core::Input::CURSOR_TEXT);
    }

    void TextField::on_mouse_leave() {
        if (!readonly)
            core::Input::set_cursor(core::Input::CURSOR_DEFAULT);
    }

    void TextField::on_focus_lost() {
        m_cursor_pos = -1;
    }

    void TextField::clear_selection() {
        if (m_selection_start != -1 && m_selection_end != -1) {
            if (m_selection_start > m_selection_end)
                std::swap(m_selection_start, m_selection_end);
            else
                m_selection_end++;
            m_text.erase(m_text.begin() + m_selection_start, m_text.begin() + m_selection_end);
            m_cursor_pos = m_selection_start;
            m_selection_start = -1;
            m_selection_end = -1;
            m_selecting = false;
        }
    }

    void TextField::late_update() {
        Widget::late_update();
        if (m_changed) {
            execute_callbacks("change");
            m_changed = false;
        }
    }

}
