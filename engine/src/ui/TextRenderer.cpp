#include "ui/TextRenderer.hpp"

#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "ui/Rectangle.hpp"
#include <ft2build.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include FT_FREETYPE_H

namespace Birdy3d {

    TextRenderer::~TextRenderer() {
        FT_Done_Face(*m_face);
        FT_Done_FreeType(*m_ft);
        free(m_face);
        free(m_ft);
    }

    TextRenderer::TextRenderer(std::string path, unsigned int fontSize) {
        m_fontSize = fontSize;
        m_ft = (FT_Library*)malloc(sizeof(FT_Library));
        m_face = (FT_Face*)malloc(sizeof(FT_Face));
        if (FT_Init_FreeType(m_ft))
            Logger::error("freetype: Could not init FreeType Library");
        if (FT_New_Face(*m_ft, path.c_str(), 0, m_face))
            Logger::error("freetype: Failed to load font");
        FT_Set_Pixel_Sizes(*m_face, 0, fontSize);
        m_rect = std::make_unique<Rectangle>(UIVector(0), UIVector(0), Color::WHITE, Rectangle::Type::TEXT);
    }

    bool TextRenderer::addChar(char32_t c) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        if (FT_Load_Char(*m_face, c, FT_LOAD_RENDER)) {
            Logger::warn("freetype: Failed to load Glyph ", (unsigned)c);
            return false;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (*m_face)->glyph->bitmap.width, (*m_face)->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, (*m_face)->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // store character
        Character character = {
            texture,
            glm::ivec2((*m_face)->glyph->bitmap.width, (*m_face)->glyph->bitmap.rows),
            glm::ivec2((*m_face)->glyph->bitmap_left, (*m_face)->glyph->bitmap_top),
            (*m_face)->glyph->advance.x
        };
        m_chars.insert(std::pair<char, Character>(c, character));
        return true;
    }

    void TextRenderer::renderText(std::string text, float x, float y, float fontSize, Color color) {
        glm::vec2 viewportSize = Application::getViewportSize();
        glm::mat4 m = glm::ortho(0.0f, viewportSize.x, 0.0f, viewportSize.y);
        renderText(text, x, y, fontSize, color, m);
    }

    void TextRenderer::renderText(std::string text, float x, float y, float fontSize, Color color, glm::mat4 move, int cursorpos, bool highlight, int hlstart, int hlend, Color hlcolor) {
        std::u32string converted = converter.from_bytes(text);
        renderText(converted, x, y, fontSize, color, move, cursorpos, highlight, hlstart, hlend, hlcolor);
    }

    void TextRenderer::renderText(std::u32string text, float x, float y, float fontSize, Color color, glm::mat4 move, int cursorpos, bool highlight, int hlstart, int hlend, Color hlcolor) {
        if (hlstart > hlend) {
            std::swap(hlstart, hlend);
            hlend--;
        }

        bool highlighting = highlight && hlstart < 0;
        float scale = (fontSize / m_fontSize);
        char16_t c;
        for (int i = 0; i <= (int)text.length(); i++) {
            if (i < (int)text.length())
                c = text[i];
            else
                c = ' ';

            if (highlight && i == hlstart)
                highlighting = true;
            if (hlend >= 0 && i == hlend + 1)
                highlighting = false;

            if (m_chars.count(c) == 0) {
                addChar(c);
            }
            Character ch = m_chars[c];
            float bottomToOrigin = (ch.size.y - ch.bearing.y) / 1.5; // For some reason, the division by 1.5 is necessary.
            float xpos = x + ch.bearing.x * scale;
            float ypos = y;
            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            m_rect->position(UIVector(xpos, ypos - bottomToOrigin));
            m_rect->texture(ch.textureID);
            m_rect->color(color);

            m_rect->size(UIVector(w, h));
            m_rect->texCoords(glm::vec2(0, 0), glm::vec2(1, 1));

            m_rect->draw(move);

            if (i == cursorpos) {
                m_rect->type = Rectangle::FILLED;
                m_rect->position(UIVector(xpos - 2, ypos - fontSize / 5));
                m_rect->size(UIVector(2, fontSize));
                m_rect->draw(move);

                m_rect->type = Rectangle::TEXT;
            }

            if (highlighting && i < (int)text.length()) {
                m_rect->type = Rectangle::FILLED;
                m_rect->color(hlcolor);
                m_rect->position(UIVector(xpos, ypos - fontSize / 5));
                m_rect->size(UIVector((ch.advance >> 6) * scale, fontSize));
                m_rect->draw(move);

                m_rect->color(color);
                m_rect->type = Rectangle::TEXT;
            }

            x += (ch.advance >> 6) * scale;
        }
    }

    UIVector TextRenderer::textSize(std::string text, float fontSize) {
        std::u32string converted = converter.from_bytes(text);
        return textSize(converted, fontSize);
    }

    UIVector TextRenderer::textSize(std::u32string text, float fontSize) {
        float scale = (fontSize / m_fontSize);
        UIVector size(0_px);
        for (std::u32string::const_iterator c = text.begin(); c != text.end(); c++) {
            if (m_chars.count(*c) == 0)
                addChar(*c);
            Character ch = m_chars[*c];
            float h = ch.size.y * scale;
            size.y = std::max((float)size.y, h);
            size.x += (ch.advance >> 6) * scale;
        }
        return size;
    }

    float TextRenderer::charWidth(char32_t c, float fontSize) {
        if (m_chars.count(c) == 0)
            addChar(c);
        Character ch = m_chars[c];
        return (ch.advance >> 6) * (fontSize / m_fontSize);
    }

    int TextRenderer::char_index(std::u32string text, float font_size, float x_pos, bool between_chars) {
        float width = 0;
        float current_char_width;
        for (size_t i = 0; i < text.size(); i++) {
            width += current_char_width = charWidth(text[i], font_size);
            if ((between_chars && x_pos < width - current_char_width / 2) || (!between_chars && x_pos < width))
                return i;
        }
        return text.size();
    }

    void Text::draw(glm::mat4 move) {
        if (m_hidden)
            return;

        if (m_dirty) {
            UIVector textSize = renderer->textSize(text, fontSize);
            m_relativePos = Utils::getRelativePosition(m_position, textSize, m_parentSize, m_placement);
            m_dirty = false;
        }
        renderer->renderText(text, m_relativePos.x, m_relativePos.y, fontSize, m_color, move);
    }

    bool Text::contains(glm::vec2) {
        return false;
    }

}
