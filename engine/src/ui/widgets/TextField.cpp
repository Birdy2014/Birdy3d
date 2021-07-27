#include "ui/widgets/TextField.hpp"

#include "core/Input.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    TextField::TextField(UIVector position, UIVector size, Placement placement)
        : Widget(position, size, placement) {
        add_filled_rectangle(0_px, 100_p, theme->color_input_bg);
    }

    std::string TextField::text() {
        return theme->text_renderer()->converter.to_bytes(m_text);
    }

    void TextField::text(std::string text) {
        std::u32string new_text = theme->text_renderer()->converter.from_bytes(text);
        if (new_text != m_text)
            m_changed = true;
        m_text = new_text;
    }

    void TextField::append(std::string text) {
        m_changed = true;
        m_text += theme->text_renderer()->converter.from_bytes(text);
    }

    void TextField::draw() {
        Widget::draw();
        glEnable(GL_SCISSOR_TEST);
        glScissor(m_actual_pos.x, m_actual_pos.y, m_actual_size.x, m_actual_size.y);
        theme->text_renderer()->renderText(m_text, 0, m_actual_size.y / 2 - theme->font_size / 2, theme->font_size, theme->color_fg, normalizedMove(), m_cursor_pos, m_selection_start != -1 && m_selection_end != -1, m_selection_start, m_selection_end, theme->color_text_highlight);
        glDisable(GL_SCISSOR_TEST);
    }

    void TextField::on_update() {
        if (m_selecting) {
            glm::vec2 local_pos = Input::cursorPos() - m_actual_pos;
            int char_pos = theme->text_renderer()->char_index(m_text, theme->font_size, local_pos.x, true);
            if (m_selection_start == char_pos)
                m_selection_end = -1;
            else
                m_selection_end = char_pos + (m_selection_start < char_pos ? -1 : 0);
        }
    }

    void TextField::on_click(InputClickEvent* event) {
        if (readonly || event->button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        if (event->action == GLFW_PRESS) {
            grab_cursor();
            glm::vec2 local_pos = Input::cursorPos() - m_actual_pos;
            int char_pos = theme->text_renderer()->char_index(m_text, theme->font_size, local_pos.x, true);
            m_selecting = true;
            m_selection_start = char_pos;
            m_cursor_pos = -1;
        } else if (event->action == GLFW_RELEASE && m_selection_end == -1) {
            ungrab_cursor();
            m_selecting = false;
            m_cursor_pos = m_selection_start;
            m_selection_start = -1;
            m_selection_end = -1;
        } else if (event->action == GLFW_RELEASE) {
            ungrab_cursor();
            m_selecting = false;
        }
    }

    void TextField::on_key(InputKeyEvent* event) {
        if (readonly || event->action != GLFW_PRESS)
            return;

        if (m_selection_start >= 0 && m_selection_end >= 0) {
            if (event->key == GLFW_KEY_DELETE || event->key == GLFW_KEY_BACKSPACE)
                clear_selection();
            return;
        }

        if (m_cursor_pos >= 0) {
            switch (event->key) {
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

    void TextField::on_char(InputCharEvent* event) {
        if (readonly)
            return;

        clear_selection();

        if (m_cursor_pos < 0 || m_cursor_pos > (int)m_text.size())
            return;

        char32_t c[2];
        c[0] = event->codepoint;
        c[1] = 0;
        m_text.insert(m_cursor_pos, c);
        m_cursor_pos++;
    }

    void TextField::on_mouse_enter() {
        if (!readonly)
            Input::setCursor(Input::CURSOR_TEXT);
    }

    void TextField::on_mouse_leave() {
        if (!readonly)
            Input::setCursor(Input::CURSOR_DEFAULT);
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
        if (m_changed && callback_change) {
            callback_change();
            m_changed = false;
        }
    }

}
