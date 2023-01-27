#include "ui/widgets/TextField.hpp"

#include "core/Input.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d::ui {

    glm::ivec2 TextField::minimal_size()
    {
        return {1.0f, core::Application::theme().line_height()};
    }

    std::string TextField::text()
    {
        return m_text.text();
    }

    void TextField::text(std::string text)
    {
        m_text.text(text);
    }

    void TextField::append(std::string text)
    {
        m_changed = true;
        m_text.append(text);
    }

    void TextField::clear()
    {
        m_text.clear();
        m_selecting = false;
        m_highlight_visible = false;
        m_changed = true;
    }

    void TextField::scroll_down()
    {
        m_scroll_offset.y = m_max_scroll_offset.y;
    }

    void TextField::on_update()
    {
        Scrollable::on_update();
        if (m_selecting) {
            auto local_pos = core::Input::cursor_pos_int() - m_absolute_rect.position();
            auto scrolled_text_local_pos = local_pos - m_scroll_offset;
            auto char_pos = char_index(glm::ivec2(scrolled_text_local_pos.x - m_side_padding, scrolled_text_local_pos.y));
            if (m_highlight_start == char_pos) {
                m_highlight_visible = false;
            } else {
                if (m_highlight_start < char_pos)
                    m_highlight_end = char_pos.previous_in(m_text);
                else
                    m_highlight_end = char_pos;
                m_highlight_visible = true;
                scroll_if_needed(m_highlight_end);
            }
        }
    }

    void TextField::on_click(ClickEvent& event)
    {
        if (multiline)
            Scrollable::on_click(event);

        if (readonly || event.button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        event.handled();

        if (event.action == GLFW_PRESS) {
            grab_cursor();
            auto local_pos = core::Input::cursor_pos_int() - m_absolute_rect.position();
            auto scrolled_text_local_pos = local_pos - m_scroll_offset;
            auto char_pos = char_index(glm::ivec2(scrolled_text_local_pos.x - m_side_padding, scrolled_text_local_pos.y));
            m_selecting = true;
            m_highlight_start = char_pos;
            m_cursor_visible = false;
        } else if (event.action == GLFW_RELEASE && !m_highlight_visible) {
            ungrab_cursor();
            m_selecting = false;
            m_cursor_visible = true;
            m_cursor_pos = m_highlight_start;
            m_highlight_visible = false;
        } else if (event.action == GLFW_RELEASE) {
            ungrab_cursor();
            m_selecting = false;
        }
    }

    void TextField::on_key(KeyEvent& event)
    {
        if (readonly || (event.action != GLFW_PRESS && event.action != GLFW_REPEAT))
            return;

        // FIXME: Enter creates newline in multiline textfield
        if (event.key == GLFW_KEY_ENTER) {
            if (on_accept)
                std::invoke(on_accept);
            return;
        }

        if (m_highlight_visible) {
            if (event.key == GLFW_KEY_DELETE || event.key == GLFW_KEY_BACKSPACE)
                clear_selection();
            return;
        }

        if (m_cursor_visible) {
            switch (event.key) {
            case GLFW_KEY_DELETE:
                if (m_cursor_pos == m_text.end())
                    break;
                m_text.erase_char(m_cursor_pos);
                m_changed = true;
                break;
            case GLFW_KEY_BACKSPACE:
                if (m_cursor_pos.is_zero())
                    break;
                m_cursor_pos = m_cursor_pos.previous_in(m_text);
                m_text.erase_char(m_cursor_pos);
                m_changed = true;
                break;
            case GLFW_KEY_LEFT:
                if (m_cursor_pos.is_zero())
                    break;
                m_cursor_pos = m_cursor_pos.previous_in(m_text);
                break;
            case GLFW_KEY_RIGHT:
                m_cursor_pos = m_cursor_pos.next_in(m_text);
                break;
            case GLFW_KEY_UP: {
                if (!multiline)
                    break;
                auto const cursor_pixel_pos = coordinate_of_index(m_cursor_pos);
                auto const line_height = core::Application::theme().line_height();
                m_cursor_pos = char_index({cursor_pixel_pos.x, cursor_pixel_pos.y - line_height * 2.0f});
                break;
            }
            case GLFW_KEY_DOWN: {
                if (!multiline)
                    break;
                auto const cursor_pixel_pos = coordinate_of_index(m_cursor_pos);
                m_cursor_pos = char_index({cursor_pixel_pos.x, cursor_pixel_pos.y});
                break;
            }
            }
        }
        scroll_if_needed(m_cursor_pos);
    }

    void TextField::draw()
    {
        auto& theme = core::Application::theme();
        auto color_fg = theme.color(utils::Color::Name::FG);
        auto color_input_bg = theme.color(utils::Color::Name::BG_INPUT);
        paint_background(color_input_bg);

        paint_text(-m_scroll_offset, m_text);

        if (m_cursor_visible) {
            auto cursor_screen_position = coordinate_of_index(m_cursor_pos) - glm::ivec2(1, theme.line_height());

            auto cursor_rectangle = Rect::from_position_and_size(cursor_screen_position, glm::ivec2(2, m_text.font_size()));
            paint_rectangle_filled(cursor_rectangle, color_fg);
        }

        if (m_highlight_visible) {
            auto color_highlight = theme.color(utils::Color::Name::TEXT_HIGHLIGHT);

            auto hlstart = m_highlight_start;
            auto hlend = m_highlight_end;
            if (hlstart > m_text.end())
                hlstart = m_text.end();
            if (hlend > m_text.end())
                hlend = m_text.end();

            if (hlstart != hlend.next_in(m_text)) {
                if (hlstart > hlend) {
                    std::swap(hlstart, hlend);
                    hlend = hlend.previous_in(m_text);
                }

                for (auto row = hlstart.row; row <= hlend.row; ++row) {
                    auto start_column = row == hlstart.row ? hlstart.column : 0;
                    auto end_column = row == hlend.row ? hlend.column : m_text.lines()[row].length() - 1;

                    auto hlstart_pos = coordinate_of_index(TextDescription::Position{.row = row, .column = start_column}) - glm::ivec2(0, theme.line_height() + 2);
                    auto hlend_pos = coordinate_of_index(TextDescription::Position{.row = row, .column = end_column + 1}) - glm::ivec2(0, 2);

                    auto highlight_rectangle = Rect::from_top_left_and_bottom_right(hlstart_pos, hlend_pos);

                    paint_rectangle_filled(highlight_rectangle, color_highlight);
                }
            }
        }

        if (multiline)
            Scrollable::draw();
    }

    void TextField::on_char(CharEvent& event)
    {
        if (readonly)
            return;

        clear_selection();

        if (!m_cursor_visible || (!m_text.contains(m_cursor_pos) && m_cursor_pos != m_text.end()))
            return;

        char32_t c[2];
        c[0] = event.codepoint;
        c[1] = 0;
        m_text.insert_u32(m_cursor_pos, c);
        m_cursor_pos.column++;
        m_changed = true;
        scroll_if_needed(m_cursor_pos);
    }

    void TextField::on_mouse_enter(MouseEnterEvent&)
    {
        if (!readonly)
            core::Input::set_cursor(core::Input::CURSOR_TEXT);
    }

    void TextField::on_mouse_leave(MouseLeaveEvent&)
    {
        if (!readonly)
            core::Input::set_cursor(core::Input::CURSOR_DEFAULT);
    }

    void TextField::on_focus_lost(FocusLostEvent&)
    {
        m_cursor_visible = false;
    }

    void TextField::clear_selection()
    {
        if (!m_highlight_visible)
            return;

        if (m_highlight_start > m_highlight_end)
            std::swap(m_highlight_start, m_highlight_end);
        else
            m_highlight_end = m_highlight_end.next_in(m_text);

        m_text.erase(m_highlight_start, m_highlight_end.previous_in(m_text));
        m_cursor_visible = true;
        m_cursor_pos = m_highlight_start;
        m_highlight_visible = false;
        m_selecting = false;
    }

    void TextField::late_update()
    {
        Widget::late_update();
        if (m_changed) {
            if (on_change)
                std::invoke(on_change);
            m_changed = false;
        }
    }

    void TextField::scroll_if_needed(TextDescription::Position cursor_pos)
    {
        auto const cursor_pixel_pos = coordinate_of_index(cursor_pos) + m_scroll_offset;
        auto const line_height = core::Application::theme().line_height();
        // Scroll left
        if (cursor_pixel_pos.x < 0)
            m_scroll_offset.x -= cursor_pixel_pos.x;
        // Scroll right
        if (cursor_pixel_pos.x > m_absolute_rect.width())
            m_scroll_offset.x -= cursor_pixel_pos.x - m_absolute_rect.width();
        // Scroll up
        if (cursor_pixel_pos.y - line_height < 0)
            m_scroll_offset.y -= cursor_pixel_pos.y - line_height;
        // Scroll down
        if (cursor_pixel_pos.y > m_absolute_rect.height())
            m_scroll_offset.y -= cursor_pixel_pos.y - m_absolute_rect.height();
    }

    glm::ivec2 TextField::content_size()
    {
        return m_text.text_size();
    }

    TextDescription::Position TextField::char_index(glm::ivec2 pos)
    {
        float width = 0;
        float height = core::Application::theme().line_height();
        float current_char_width;
        for (TextDescription::Position i; i < m_text.end(); i = i.next_in(m_text)) {
            auto optional_current = m_text.at_u32(i);
            assert(optional_current.has_value());
            auto current_char = optional_current.value();

            if (current_char == U'\x1B') {
                i = i.next_in(m_text); // Go to color
                if (i >= m_text.end())
                    break;
                continue;
            }
            if (current_char == U'\n') {
                if (pos.y < height)
                    return i;
                width = 0;
                height += core::Application::theme().line_height();
                continue;
            }
            width += current_char_width = core::Application::theme().text_renderer().char_width(current_char, m_text.font_size());
            if (pos.y < height && (pos.x < width - current_char_width / 2))
                return i;
        }
        return m_text.end();
    }

    glm::ivec2 TextField::coordinate_of_index(TextDescription::Position index)
    {
        auto const line_height = core::Application::theme().line_height();
        glm::ivec2 pos = {0, line_height * (index.row + 1)};
        for (TextDescription::Position current_position{.row = index.row}; m_text.contains(current_position) && current_position.column < index.column; ++current_position.column) {
            auto optional_current = m_text.at_u32(current_position);
            assert(optional_current.has_value());
            auto current_char = optional_current.value();

            if (current_char == U'\x1B') {
                current_position = current_position.next_in(m_text); // Go to color
                if (!m_text.contains(current_position) || current_position.column >= index.column)
                    break;
                continue;
            }
            pos.x += core::Application::theme().text_renderer().char_width(current_char, m_text.font_size());
        }
        return pos;
    }

}
