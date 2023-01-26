#pragma once

#include "core/Application.hpp"
#include "ui/Theme.hpp"
#include "utils/Unicode.hpp"
#include <ranges>

namespace Birdy3d::ui {

    // FIXME: Rename to "Text" once "Text" is removed from TextRenderer
    class TextDescription {
    public:
        TextDescription() = default;

        TextDescription(std::u32string text, unsigned int font_size = 0)
            : m_font_size(font_size)
        {
            text_u32(text);
        }

        TextDescription(std::string text, unsigned int font_size = 0)
            : m_font_size(font_size)
        {
            this->text(text);
        }

        [[nodiscard]] std::vector<std::u32string> const& lines() const
        {
            return m_lines;
        }

        [[nodiscard]] std::u32string text_u32() const
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

        void text_u32(std::u32string text)
        {
            m_lines.clear();
            for (auto const& line : std::ranges::views::split(std::u32string_view(text), std::u32string_view(U"\n"))) {
                m_lines.push_back(std::u32string(line.begin(), line.end()));
            }
            m_text_size_dirty = true;
        }

        [[nodiscard]] std::string text() const
        {
            return utils::Unicode::utf32_to_utf8(text_u32());
        }

        void text(std::string text)
        {
            text_u32(utils::Unicode::utf8_to_utf32(text));
        }

        [[nodiscard]] unsigned int font_size() const
        {
            if (m_font_size == 0)
                return core::Application::theme().font_size();
            return m_font_size;
        }

        void font_size(unsigned int font_size) { m_font_size = font_size; }

        [[nodiscard]] glm::ivec2 text_size() const
        {
            if (m_text_size_dirty) {
                m_text_size_dirty = false;
                m_text_size = core::Application::theme().text_renderer().text_size(text_u32()).to_pixels();
            }
            return m_text_size;
        }

    private:
        std::vector<std::u32string> m_lines;
        unsigned int m_font_size{0};

        mutable bool m_text_size_dirty{true};
        mutable glm::ivec2 m_text_size;
    };

};
