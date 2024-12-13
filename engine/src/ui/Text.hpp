#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <vector>

namespace Birdy3d::ui {

    // FIXME: Rename to "Text" once "Text" is removed from TextRenderer
    class TextDescription {
    public:
        struct Position {
            std::size_t row = 0;
            std::size_t column = 0;

            bool operator==(Position const& other) const;
            std::strong_ordering operator<=>(Position const& other) const;

            [[nodiscard]] bool is_zero() const;
            [[nodiscard]] Position previous_in(TextDescription const& text) const;
            [[nodiscard]] Position next_in(TextDescription const& text) const;
        };

        TextDescription() = default;
        TextDescription(std::u32string text, unsigned int font_size = 0);
        TextDescription(std::string text, unsigned int font_size = 0);

        void clear();

        [[nodiscard]] std::optional<char32_t> at_u32(Position position) const;

        [[nodiscard]] std::vector<std::u32string> const& lines() const;

        [[nodiscard]] std::u32string text_u32() const;
        void text_u32(std::u32string text);
        [[nodiscard]] std::string text() const;
        void text(std::string text);

        void append_u32(std::u32string_view text);
        void append(std::string const& text);
        void append_line_u32(std::u32string_view text);
        void append_line(std::string const& text);

        bool insert_u32(Position position, std::u32string_view text);
        bool insert(Position position, std::string const& text);

        void erase_char(Position const& position);

        /**
         * Erase range from start (inclusive) to end (exclusive).
         */
        void erase(Position const& start, Position const& end);

        [[nodiscard]] unsigned int font_size() const;
        void font_size(unsigned int font_size) { m_font_size = font_size; }
        [[nodiscard]] glm::ivec2 text_size() const;

        [[nodiscard]] Position end() const;
        [[nodiscard]] bool contains(Position const& position) const;

    private:
        std::vector<std::u32string> m_lines;
        unsigned int m_font_size{0};
        bool m_should_append_newline = true;

        mutable bool m_text_size_dirty{true};
        mutable glm::ivec2 m_text_size;
    };

};
