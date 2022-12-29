#pragma once

#include "core/Application.hpp"
#include "ui/Theme.hpp"
#include "utils/Unicode.hpp"
#include <string>

namespace Birdy3d::ui {

    // FIXME: Rename to "Text" once "Text" is removed from TextRenderer
    class TextDescription {
    public:
        TextDescription()
            : m_text({})
            , m_font_size(0)
        { }

        TextDescription(std::u32string text, unsigned int font_size = 0)
            : m_text(text)
            , m_font_size(font_size)
        { }

        TextDescription(std::string text, unsigned int font_size = 0)
            : m_text(utils::Unicode::utf8_to_utf32(text))
            , m_font_size(font_size)
        { }

        [[nodiscard]] std::u32string text_u32() const
        {
            return m_text;
        }

        void text_u32(std::u32string text)
        {
            m_text = std::move(text);
            m_text_size_dirty = true;
        }

        [[nodiscard]] std::string text() const
        {
            return utils::Unicode::utf32_to_utf8(m_text);
        }

        void text(std::string text)
        {
            m_text = utils::Unicode::utf8_to_utf32(text);
            m_text_size_dirty = true;
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
                m_text_size = core::Application::theme().text_renderer().text_size(m_text).to_pixels();
            }
            return m_text_size;
        }

    private:
        std::u32string m_text;
        unsigned int m_font_size;

        mutable bool m_text_size_dirty{true};
        mutable glm::ivec2 m_text_size;
    };

};
