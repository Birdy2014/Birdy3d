#include "ui/Text.hpp"

#include "core/Application.hpp"
#include "ui/Theme.hpp"
#include "utils/Unicode.hpp"

namespace Birdy3d::ui {

    bool TextDescription::Position::operator==(TextDescription::Position const& other) const
    {
        return row == other.row && column == other.column;
    }

    std::strong_ordering TextDescription::Position::operator<=>(TextDescription::Position const& other) const
    {
        if (row < other.row || column < other.column)
            return std::strong_ordering::less;

        if (row > other.row || column > other.column)
            return std::strong_ordering::greater;

        return std::strong_ordering::equal;
    }

    [[nodiscard]] bool TextDescription::Position::is_zero() const
    {
        return row == 0 && column == 0;
    }

    [[nodiscard]] TextDescription::Position TextDescription::Position::previous_in(TextDescription const& text) const
    {
        // FIXME: Handle invalid input

        if (column > 0)
            return Position{.row = row, .column = column - 1};

        if (row > 0)
            return Position{.row = row - 1, .column = text.m_lines[row - 1].length()};

        return Position{};
    }

    [[nodiscard]] TextDescription::Position TextDescription::Position::next_in(TextDescription const& text) const
    {
        if (row < text.m_lines.size() && column < text.m_lines[row].length())
            return Position{.row = row, .column = column + 1};

        if (row + 1 < text.lines().size())
            return Position{.row = row + 1, .column = 0};

        if (row + 1 == text.lines().size() && text.m_should_append_newline)
            return Position{.row = row + 1, .column = 0};

        return *this;
    }

    TextDescription::TextDescription(std::u32string text, unsigned int font_size)
        : m_font_size(font_size)
    {
        text_u32(text);
    }

    TextDescription::TextDescription(std::string text, unsigned int font_size)
        : m_font_size(font_size)
    {
        this->text(text);
    }

    void TextDescription::clear()
    {
        m_lines.clear();
    }

    [[nodiscard]] std::optional<char32_t> TextDescription::at_u32(TextDescription::Position position) const
    {
        if (!contains(position))
            return {};
        if (position.column == m_lines[position.row].length())
            return U'\n';
        return m_lines[position.row][position.column];
    }

    [[nodiscard]] std::vector<std::u32string> const& TextDescription::lines() const
    {
        return m_lines;
    }

    [[nodiscard]] std::u32string TextDescription::text_u32() const
    {
        std::u32string text;
        for (auto it = m_lines.cbegin(); it != m_lines.cend(); ++it) {
            if (it + 1 != m_lines.cend())
                text.append(*it + U'\n');
            else
                text.append(*it);
        }
        return text;
    }

    void TextDescription::text_u32(std::u32string text)
    {
        m_lines.clear();
        append_line_u32(text);
    }

    [[nodiscard]] std::string TextDescription::text() const
    {
        return utils::Unicode::utf32_to_utf8(text_u32());
    }

    void TextDescription::text(std::string text)
    {
        text_u32(utils::Unicode::utf8_to_utf32(text));
    }

    void TextDescription::append_u32(std::u32string_view text)
    {
        auto line_begin = text.cbegin();
        for (auto char_iterator = text.cbegin();; ++char_iterator) {
            if (*char_iterator != U'\n' && char_iterator != text.cend())
                continue;

            if (m_should_append_newline || m_lines.empty()) {
                m_lines.push_back(std::u32string(line_begin, char_iterator));
            } else {
                m_lines.back().append(std::u32string_view(line_begin, char_iterator));
            }

            if (char_iterator == text.cend()) {
                m_should_append_newline = false;
                break;
            }
            ++char_iterator;
            if (char_iterator == text.cend()) {
                m_should_append_newline = true;
                break;
            }

            line_begin = char_iterator;
        }
        m_text_size_dirty = true;
    }

    void TextDescription::append(std::string const& text)
    {
        append_u32(utils::Unicode::utf8_to_utf32(text));
    }

    void TextDescription::append_line_u32(std::u32string_view text)
    {
        m_should_append_newline = true;
        append_u32(text);
    }

    void TextDescription::append_line(std::string const& text)
    {
        append_line_u32(utils::Unicode::utf8_to_utf32(text));
    }

    bool TextDescription::insert_u32(TextDescription::Position position, std::u32string_view text)
    {
        if (position.row < m_lines.size()) {
            auto& line = m_lines[position.row];

            if (position.column < line.length()) {
                line.insert(position.column, text);
            }

            if (position.column == line.length()) {
                line.append(text);
            }

            return false;
        }

        if (position.row == m_lines.size() && position.column == 0) {
            m_lines.emplace_back(text);
        }

        return false;
    }

    bool TextDescription::insert(TextDescription::Position position, std::string const& text)
    {
        return insert_u32(position, utils::Unicode::utf8_to_utf32(text));
    }

    void TextDescription::erase_char(TextDescription::Position const& position)
    {
        // FIXME: Handle invalid input

        if (position.column == m_lines[position.row].length()) {
            // Erase newline
            m_lines[position.row] += m_lines[position.row + 1];
            m_lines.erase(m_lines.begin() + position.row + 1);
            return;
        }

        m_lines[position.row].erase(position.column, 1);
    }

    /**
     * Erase range from start to end (inclusive).
     */
    void TextDescription::erase(TextDescription::Position const& start, TextDescription::Position const& end)
    {
        // FIXME: Handle invalid input

        // 1. Erase partial lines
        if (start.row == end.row) {
            // Erase in single line
            m_lines[start.row].erase(start.column, end.column - start.column + 1);
            return;
        }

        if (start.column > 0) {
            // Erase end of first line
            m_lines[start.row].erase(start.column);
        }

        if (end.column > 0) {
            auto has_next_line = end.row + 1 < m_lines.size() || m_should_append_newline;

            // Erase start of last line
            if (end.column == m_lines[end.row].length() || (end.column + 1 == m_lines[end.row].length() && !has_next_line)) {
                // Erase last line
                if (end.row + 1 == m_lines.size())
                    m_should_append_newline = false;
                m_lines.erase(m_lines.begin() + end.row);
            } else {
                m_lines[end.row].erase(0, end.column + 1);
            }
        }

        // 2. Erase full lines
        for (auto row = start.row + 1; row < end.row; ++row) {
            m_lines.erase(m_lines.begin() + start.row + 1);
        }

        if (start.column == 0) {
            // Erase first line
            m_lines.erase(m_lines.begin() + start.row);
            return;
        }

        // 3. Join line ends if necessary
        if (start.column > 0 && start.row + 1 < m_lines.size()) {
            m_lines[start.row] += m_lines[start.row + 1];
            m_lines.erase(m_lines.begin() + start.row + 1);
        }
    }

    [[nodiscard]] unsigned int TextDescription::font_size() const
    {
        if (m_font_size == 0)
            return core::Application::theme().font_size();
        return m_font_size;
    }

    [[nodiscard]] glm::ivec2 TextDescription::text_size() const
    {
        if (m_text_size_dirty) {
            m_text_size_dirty = false;
            m_text_size = core::Application::theme().text_renderer().text_size(text_u32()).to_pixels();
        }
        return m_text_size;
    }

    [[nodiscard]] TextDescription::Position TextDescription::end() const
    {
        if (m_should_append_newline)
            return TextDescription::Position{.row = m_lines.size(), .column = 0};

        if (m_lines.empty())
            return TextDescription::Position{.row = 0, .column = 0};

        return TextDescription::Position{.row = m_lines.size() - 1, .column = m_lines.back().length()};
    }

    [[nodiscard]] bool TextDescription::contains(TextDescription::Position const& position) const
    {
        if (position.row >= m_lines.size())
            return false;
        if (position.row + 1 == m_lines.size() && position.column == m_lines[position.row].length() && !m_should_append_newline)
            return false;
        return position.column <= m_lines[position.row].length();
    }

}
