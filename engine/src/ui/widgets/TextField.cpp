#include "ui/widgets/TextField.hpp"

#include "core/Input.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "utils/Unicode.hpp"

namespace Birdy3d::ui {

    TextField::TextField(Options options)
        : Scrollable(options) {
        add_filled_rectangle(0_px, 100_p, utils::Color::Name::BG_INPUT);
        m_text = add_text(0_px, std::string(), utils::Color::Name::FG);
    }

    glm::vec2 TextField::minimal_size() {
        return { 1.0f, core::Application::theme().line_height() };
    }

    std::string TextField::text() {
        return *m_text;
    }

    void TextField::text(std::string text) {
        std::u32string new_text = utils::Unicode::utf8_to_utf32(text);
        if (new_text != *m_text)
            m_changed = true;
        *m_text = new_text;
    }

    void TextField::append(std::string text) {
        m_changed = true;
        *m_text += text;
    }

    void TextField::clear() {
        m_text->clear();
        m_text->cursor_pos = 0;
        m_selecting = false;
        m_text->highlight_visible = false;
        m_text->highlight_start = 0;
        m_text->highlight_end = 0;
        m_changed = true;
    }

    void TextField::scroll_down() {
        m_scroll_offset.y = m_max_scroll_offset.y;
    }

    void TextField::on_update() {
        Scrollable::on_update();
        if (m_selecting) {
            glm::vec2 local_pos = core::Input::cursor_pos() - m_actual_pos;
            glm::vec2 scrolled_text_local_pos = local_pos - m_scroll_offset;
            auto char_pos = char_index(glm::vec2(scrolled_text_local_pos.x - m_side_padding, scrolled_text_local_pos.y));
            if (m_text->highlight_start == char_pos) {
                m_text->highlight_visible = false;
            } else {
                m_text->highlight_end = char_pos + (m_text->highlight_start < char_pos ? -1 : 0);
                m_text->highlight_visible = true;
                scroll_if_needed(m_text->highlight_end);
            }
        }
    }

    void TextField::on_click(const events::InputClickEvent& event) {
        if (multiline)
            Scrollable::on_click(event);

        if (readonly || event.button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        if (event.action == GLFW_PRESS) {
            grab_cursor();
            glm::vec2 local_pos = core::Input::cursor_pos() - m_actual_pos;
            glm::vec2 scrolled_text_local_pos = local_pos - m_scroll_offset;
            auto char_pos = char_index(glm::vec2(scrolled_text_local_pos.x - m_side_padding, scrolled_text_local_pos.y));
            m_selecting = true;
            m_text->highlight_start = char_pos;
            m_text->cursor_visible = false;
        } else if (event.action == GLFW_RELEASE && !m_text->highlight_visible) {
            ungrab_cursor();
            m_selecting = false;
            m_text->cursor_visible = true;
            m_text->cursor_pos = m_text->highlight_start;
            m_text->highlight_visible = false;
        } else if (event.action == GLFW_RELEASE) {
            ungrab_cursor();
            m_selecting = false;
        }
    }

    void TextField::on_key(const events::InputKeyEvent& event) {
        if (readonly || (event.action != GLFW_PRESS && event.action != GLFW_REPEAT))
            return;

        if (event.key == GLFW_KEY_ENTER && has_callbacks("accept"))
            return execute_callbacks("accept");

        if (m_text->highlight_visible) {
            if (event.key == GLFW_KEY_DELETE || event.key == GLFW_KEY_BACKSPACE)
                clear_selection();
            return;
        }

        if (m_text->cursor_visible) {
            switch (event.key) {
            case GLFW_KEY_DELETE:
                if (m_text->cursor_pos == m_text->length())
                    break;
                m_text->erase(m_text->cursor_pos);
                m_changed = true;
                break;
            case GLFW_KEY_BACKSPACE:
                if (m_text->cursor_pos == 0)
                    break;
                m_text->erase(m_text->cursor_pos - 1);
                m_text->cursor_pos--;
                m_changed = true;
                break;
            case GLFW_KEY_LEFT:
                if (m_text->cursor_pos == 0)
                    break;
                m_text->cursor_pos--;
                break;
            case GLFW_KEY_RIGHT:
                if (m_text->cursor_pos == m_text->length())
                    break;
                m_text->cursor_pos++;
                break;
            case GLFW_KEY_UP: {
                if (!multiline)
                    break;
                const auto cursor_pixel_pos = coordinate_of_index(m_text->cursor_pos);
                const auto line_height = core::Application::theme().line_height();
                m_text->cursor_pos = char_index({ cursor_pixel_pos.x, cursor_pixel_pos.y - line_height * 2.0f });
                break;
            }
            case GLFW_KEY_DOWN: {
                if (!multiline)
                    break;
                const auto cursor_pixel_pos = coordinate_of_index(m_text->cursor_pos);
                m_text->cursor_pos = char_index({ cursor_pixel_pos.x, cursor_pixel_pos.y });
                break;
            }
            }
        }
        scroll_if_needed(m_text->cursor_pos);
    }

    void TextField::draw() {
        if (multiline)
            Scrollable::draw();
    }

    void TextField::on_char(const events::InputCharEvent& event) {
        if (readonly)
            return;

        clear_selection();

        if (!m_text->cursor_visible || m_text->cursor_pos > m_text->length())
            return;

        char32_t c[2];
        c[0] = event.codepoint;
        c[1] = 0;
        m_text->insert(m_text->cursor_pos, c);
        m_text->cursor_pos++;
        m_changed = true;
        scroll_if_needed(m_text->cursor_pos);
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
        m_text->cursor_visible = false;
    }

    void TextField::clear_selection() {
        if (!m_text->highlight_visible)
            return;
        if (m_text->highlight_start > m_text->highlight_end)
            std::swap(m_text->highlight_start, m_text->highlight_end);
        else
            m_text->highlight_end++;
        m_text->erase(m_text->highlight_start, m_text->highlight_end);
        m_text->cursor_visible = true;
        m_text->cursor_pos = m_text->highlight_start;
        m_text->highlight_start = 0;
        m_text->highlight_end = 0;
        m_text->highlight_visible = false;
        m_selecting = false;
    }

    void TextField::late_update() {
        Widget::late_update();
        if (m_changed) {
            execute_callbacks("change");
            m_changed = false;
        }
    }

    void TextField::scroll_if_needed(std::size_t cursor_pos) {
        const auto cursor_pixel_pos = coordinate_of_index(cursor_pos) + m_scroll_offset;
        const auto line_height = core::Application::theme().line_height();
        // Scroll left
        if (cursor_pixel_pos.x < 0)
            m_scroll_offset.x -= cursor_pixel_pos.x;
        // Scroll right
        if (cursor_pixel_pos.x > m_actual_size.x)
            m_scroll_offset.x -= cursor_pixel_pos.x - m_actual_size.x;
        // Scroll up
        if (cursor_pixel_pos.y - line_height < 0)
            m_scroll_offset.y -= cursor_pixel_pos.y - line_height;
        // Scroll down
        if (cursor_pixel_pos.y > m_actual_size.y)
            m_scroll_offset.y -= cursor_pixel_pos.y - m_actual_size.y;
    }

    std::size_t TextField::char_index(glm::vec2 pos) {
        float width = 0;
        float height = core::Application::theme().line_height();
        float current_char_width;
        for (size_t i = 0; i < m_text->length(); i++) {
            if (m_text->text()[i] == '\x1B') {
                i++; // Go to color
                if (i >= m_text->length())
                    break;
                continue;
            }
            if (m_text->text()[i] == '\n') {
                if (pos.y < height)
                    return i;
                width = 0;
                height += core::Application::theme().line_height();
                continue;
            }
            width += current_char_width = core::Application::theme().text_renderer().char_width(m_text->text()[i], m_text->font_size);
            if (pos.y < height && (pos.x < width - current_char_width / 2))
                return i;
        }
        return m_text->length();
    }

    glm::vec2 TextField::coordinate_of_index(std::size_t index) {
        const auto line_height = core::Application::theme().line_height();
        glm::vec2 pos = { 0.0f, line_height };
        for (std::size_t i = 0; i < m_text->length() && i < index; ++i) {
            if (m_text->text()[i] == '\x1B') {
                i++; // Go to color
                if (i >= m_text->length() || i >= index)
                    break;
                continue;
            }
            if (m_text->text()[i] == '\n') {
                pos.x = 0.0f;
                pos.y += line_height;
                continue;
            }
            pos.x += core::Application::theme().text_renderer().char_width(m_text->text()[i], m_text->font_size);
        }
        return pos;
    }

}
