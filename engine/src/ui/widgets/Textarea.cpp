#include "ui/widgets/Textarea.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "events/InputEvents.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    Textarea::Textarea(UIVector pos, UIVector size, Placement placement)
        : TextField(pos, size, placement) {
        scrollpos = 0;
        m_tmpscroll = 0;
    }

    void Textarea::arrange(glm::vec2 pos, glm::vec2 size) {
        Widget::arrange(pos, size);
        update_lines();
    }

    void Textarea::draw() {
        Widget::draw();
        glEnable(GL_SCISSOR_TEST);
        glScissor(m_actual_pos.x, m_actual_pos.y, m_actual_size.x, m_actual_size.y);
        int linec = m_actual_size.y / Application::theme->line_height();
        size_t line;
        for (int l = 0; l < linec + 1; l++) {
            // smooth scrolling
            float scrolldelta = (scrollpos - m_tmpscroll) * Application::delta_time;
            m_tmpscroll += scrolldelta;
            if (scrolldelta < 0.0002 && scrolldelta > -0.0002)
                m_tmpscroll = scrollpos;

            // draw lines
            line = l + floor(m_tmpscroll);
            int y = m_actual_size.y - (l + 1) * Application::theme->line_height() + (m_tmpscroll - floor(m_tmpscroll)) * Application::theme->line_height();
            int selection_start = m_selection_start <= m_selection_end ? m_selection_start : m_selection_end;
            int selection_end = m_selection_start <= m_selection_end ? m_selection_end + 1 : m_selection_start;
            selection_end--;
            if (line >= 0 && line < m_lines.size()) {
                size_t line_start = line > 0 ? m_lines[line - 1] : 0;
                Application::theme->text_renderer().render_text(m_text.substr(line_start, m_lines[line] - line_start - 1), 0, y, Application::theme->font_size(), Color::Name::FG, m_move, m_cursor_pos - line_start, m_lines[line] > selection_start && line_start <= selection_end && m_selection_end != -1, selection_start - line_start, selection_end - line_start, Color::Name::TEXT_HIGHLIGHT);
            }
        }
        glDisable(GL_SCISSOR_TEST);
    }

    void Textarea::update_lines() {
        m_lines.clear();
        std::u32string line;
        size_t line_end;
        size_t pos = 0, eol = 0, nextspace = 0, prevspace = 0, length = 0;
        while (pos != std::u32string::npos && pos < m_text.length()) {
            eol = m_text.find_first_of('\n', pos);
            if (eol == std::u32string::npos)
                eol = m_text.length();

            line = m_text.substr(pos, eol - pos);
            line_end = eol;
            // Line is too long
            length = Application::theme->text_renderer().text_size(line, Application::theme->font_size()).x;
            if (length > m_actual_size.x) {
                nextspace = pos;
                while (nextspace < eol) {
                    prevspace = nextspace;
                    nextspace = m_text.find_first_of(' ', prevspace + 1);
                    line = m_text.substr(pos, nextspace - pos);
                    line_end = nextspace;

                    // reached the space too far right
                    length = Application::theme->text_renderer().text_size(line, Application::theme->font_size()).x;
                    if (length > m_actual_size.x) {
                        // the line can't be broken using a space
                        if (prevspace == pos)
                            prevspace = eol;
                        line = m_text.substr(pos, prevspace - pos);
                        line_end = prevspace;
                        break;
                    }
                }
                eol = prevspace;
            }
            pos = eol + 1;
            m_lines.push_back(line_end + 1);
        }
    }

    void Textarea::on_update() {
        if (m_selecting) {
            int char_pos = cursor_char_pos();
            if (m_selection_start == char_pos)
                m_selection_end = -1;
            else
                m_selection_end = char_pos + (m_selection_start < char_pos ? -1 : 0);
        }
    }

    void Textarea::on_click(InputClickEvent* event) {
        if (readonly)
            return;

        size_t char_pos = cursor_char_pos();

        if (event->action == GLFW_PRESS) {
            grab_cursor();
            m_selecting = true;
            m_selection_start = char_pos;
            m_cursor_pos = -1;
        } else if (event->action == GLFW_RELEASE && char_pos == m_selection_start) {
            ungrab_cursor();
            m_selecting = false;
            m_cursor_pos = m_selection_start;
            m_selection_start = -1;
            m_selection_end = -1;
        } else if (event->action == GLFW_RELEASE && char_pos != m_selection_start) {
            ungrab_cursor();
            m_selecting = false;
        }
    }

    void Textarea::on_scroll(InputScrollEvent* event) {
        scrollpos -= event->yoffset;
        if (scrollpos < 0)
            scrollpos = 0;
    }

    void Textarea::on_char(InputCharEvent* event) {
        TextField::on_char(event);
        update_lines();
    }

    // TODO: key repeat
    void Textarea::on_key(InputKeyEvent* event) {
        TextField::on_key(event);
        if (readonly || event->action != GLFW_PRESS || m_cursor_pos <= 0)
            return;
        glm::ivec2 pos = get_2d_pos(m_cursor_pos);
        switch (event->key) {
        case GLFW_KEY_UP: {
            if (m_cursor_pos < m_lines[0])
                break;
            int prev_line_length = m_lines[pos.y - 1] - (pos.y == 1 ? 0 : m_lines[pos.y - 2]) - 1;
            if (prev_line_length > pos.x)
                m_cursor_pos -= prev_line_length + 1;
            else
                m_cursor_pos -= pos.x + 1;
            break;
        }
        case GLFW_KEY_DOWN: {
            if (pos.y >= m_lines.size())
                break;
            int line_length = m_lines[pos.y] - m_lines[pos.y - 1] - 1;
            int next_line_length = m_lines[pos.y + 1] - m_lines[pos.y] - 1;
            if (next_line_length > pos.x)
                m_cursor_pos += line_length + 1;
            else
                m_cursor_pos += line_length - pos.x + next_line_length + 1;
            break;
        }
        case GLFW_KEY_ENTER: {
            m_text.insert(m_cursor_pos, U"\n");
            m_cursor_pos++;
            break;
        }
        }
        update_lines();
    }

    size_t Textarea::cursor_char_pos() {
        glm::vec2 local_pos = Input::cursor_pos() - m_actual_pos;

        int y = m_tmpscroll + (m_actual_size.y - local_pos.y) / Application::theme->line_height();
        if (y >= m_lines.size())
            y = m_lines.size() - 1;

        size_t line_start = y > 0 ? m_lines[y - 1] : 0;
        int char_pos = Application::theme->text_renderer().char_index(m_text.substr(line_start, m_lines[y] - line_start - 1), Application::theme->font_size(), local_pos.x, true);
        return line_start + char_pos;
    }

    glm::ivec2 Textarea::get_2d_pos(size_t pos) {
        for (size_t y = 0; y < m_lines.size(); y++) {
            if (m_lines[y] > pos) {
                if (y == 0)
                    return glm::ivec2(pos, y);
                else
                    return glm::ivec2(pos - m_lines[y - 1], y);
            }
        }
        return glm::vec2(m_lines.size(), 0);
    }

}
