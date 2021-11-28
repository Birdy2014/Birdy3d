#include "ui/TextRenderer.hpp"

#include "core/Logger.hpp"
#include "core/ResourceManager.hpp"
#include "render/Shader.hpp"
#include "ui/Rectangle.hpp"
#include "ui/Theme.hpp"
#include "utils/Unicode.hpp"
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

    TextRenderer::TextRenderer(Theme& theme)
        : m_theme(theme) {
        std::string path = ResourceManager::get_resource_path(theme.font(), ResourceManager::ResourceType::FONT);
        m_font_size = theme.font_size();
        m_ft = (FT_Library*)malloc(sizeof(FT_Library));
        m_face = (FT_Face*)malloc(sizeof(FT_Face));
        if (FT_Init_FreeType(m_ft))
            Logger::critical("freetype: Could not init FreeType Library");
        if (FT_New_Face(*m_ft, path.c_str(), 0, m_face))
            throw std::runtime_error("freetype: Failed to load font");
        FT_Set_Pixel_Sizes(*m_face, 0, m_font_size);
        m_rect = std::make_unique<Rectangle>(UIVector(0), UIVector(0), Color::Name::FG, Rectangle::Type::FILLED);
        m_text = std::make_unique<Text>(0_px, "", Color::Name::FG, Placement::BOTTOM_LEFT, m_font_size);

        // Setup texture atlas
        m_texture_atlas_size = glm::vec2(m_font_size * 10, m_font_size * 10);
        std::vector<unsigned char> pixels;
        pixels.resize(m_texture_atlas_size.x * m_texture_atlas_size.y);
        for (size_t i = 0; i < pixels.size(); i++)
            pixels[i] = 0;
        m_texture_atlas_current_pos = glm::ivec2(0);
        glGenTextures(1, &m_texture_atlas);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture_atlas);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_texture_atlas_size.x, m_texture_atlas_size.y, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());
    }

    bool TextRenderer::add_char(char32_t c) {
        if (FT_Load_Char(*m_face, c, FT_LOAD_RENDER)) {
            Logger::warn("freetype: Failed to load Glyph ", (unsigned)c);
            return false;
        }
        if (m_texture_atlas_current_pos.x + (*m_face)->glyph->bitmap.width > m_texture_atlas_size.x) {
            if (m_texture_atlas_current_pos.y + m_font_size > m_texture_atlas_size.y) {
                Logger::warn("Failed to load glyph '", (unsigned)c, "': font texture atlas full");
                return false;
            }
            m_texture_atlas_current_pos.x = 0;
            m_texture_atlas_current_pos.y += m_font_size;
        }
        // generate texture
        glBindTexture(GL_TEXTURE_2D, m_texture_atlas);
        glTexSubImage2D(GL_TEXTURE_2D, 0, m_texture_atlas_current_pos.x, m_texture_atlas_current_pos.y, (*m_face)->glyph->bitmap.width, (*m_face)->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, (*m_face)->glyph->bitmap.buffer);
        // store character
        Character character = {
            glm::vec2((m_texture_atlas_current_pos.x) / m_texture_atlas_size.x, (m_texture_atlas_current_pos.y) / m_texture_atlas_size.y),
            glm::vec2((m_texture_atlas_current_pos.x + (*m_face)->glyph->bitmap.width) / (float)m_texture_atlas_size.x, (m_texture_atlas_current_pos.y + (*m_face)->glyph->bitmap.rows) / m_texture_atlas_size.y),
            glm::ivec2((*m_face)->glyph->bitmap.width, (*m_face)->glyph->bitmap.rows),
            glm::ivec2((*m_face)->glyph->bitmap_left, (*m_face)->glyph->bitmap_top),
            (*m_face)->glyph->advance.x
        };
        m_chars.insert(std::pair<char, Character>(c, character));
        m_texture_atlas_current_pos.x += (*m_face)->glyph->bitmap.width + 1;
        return true;
    }

    void TextRenderer::render_text(std::string text, float x, float y, float font_size, Color::Name color, glm::mat4 move, bool cursor, std::size_t cursorpos, bool highlight, std::size_t hlstart, std::size_t hlend, Color::Name hlcolor) {
        std::u32string converted = Unicode::utf8_to_utf32(text);
        render_text(converted, x, y, font_size, color, move, cursor, cursorpos, highlight, hlstart, hlend, hlcolor);
    }

    void TextRenderer::render_text(std::u32string text, float x, float y, float font_size, Color::Name color, glm::mat4 move, bool cursor, std::size_t cursorpos, bool highlight, std::size_t hlstart, std::size_t hlend, Color::Name hlcolor) {
        // Render text
        m_text->text_u32(text);
        m_text->position(UIVector(x, y));
        m_text->font_size = font_size;
        m_text->color(color);
        m_text->draw(move);

        // Render cursor
        if (cursor && cursorpos <= text.length()) {
            auto size = text_size(text, font_size, cursorpos);
            float xpos = x + size.x - 1;
            float ypos = y - size.y + m_theme.line_height();

            m_rect->type = Rectangle::FILLED;
            m_rect->position(UIVector(xpos, ypos));
            m_rect->size(UIVector(2, font_size));
            m_rect->color(color);
            m_rect->draw(move);
        }

        // Render Hightlight
        if (hlstart > text.length())
            hlstart = text.length();
        if (hlend > text.length())
            hlend = text.length();
        if (highlight && hlstart != hlend + 1) {
            if (hlstart > hlend) {
                std::swap(hlstart, hlend);
                hlend--;
            }

            glm::vec2 pos = { x, y };
            auto hlstart_pos = pos + (glm::vec2)text_size(text, font_size, hlstart) * glm::vec2(1, -1) + glm::vec2(0, m_theme.line_height());
            auto hlend_pos = pos + (glm::vec2)text_size(text, font_size, hlend + 1) * glm::vec2(1, -1) + glm::vec2(0, m_theme.line_height());

            m_rect->type = Rectangle::FILLED;
            m_rect->position(UIVector(hlstart_pos.x, y));
            m_rect->size(UIVector(hlend_pos.x - hlstart_pos.x, font_size));
            m_rect->color(hlcolor);
            m_rect->draw(move);
        }
    }

    UIVector TextRenderer::text_size(std::string text, float font_size, std::size_t n) {
        std::u32string converted = Unicode::utf8_to_utf32(text);
        return text_size(converted, font_size, n);
    }

    UIVector TextRenderer::text_size(std::u32string text, float font_size, std::size_t n) {
        if (!font_size)
            font_size = m_font_size;
        float scale = (font_size / m_font_size);
        UIVector size(0_px, font_size);
        float current_x = 0;
        for (std::size_t i = 0; i < text.length() && i < n; i++) {
            char32_t c = text[i];
            if (c == '\n') {
                size.x = std::max(size.x.to_pixels(), current_x);
                size.y += m_theme.line_height();
                current_x = 0;
                continue;
            }
            if (c == '\x1B') {
                i++; // Go to color
                if (i >= text.length() || i >= n)
                    break;
                continue;
            }
            if (m_chars.count(c) == 0)
                add_char(c);
            Character ch = m_chars[c];
            current_x += (ch.advance >> 6) * scale;
        }
        size.x = std::max(size.x.to_pixels(), current_x);
        return size;
    }

    float TextRenderer::char_width(char32_t c, float font_size) {
        if (!font_size)
            font_size = m_font_size;
        if (m_chars.count(c) == 0)
            add_char(c);
        Character ch = m_chars[c];
        return (ch.advance >> 6) * (font_size / m_font_size);
    }

    int TextRenderer::char_index(std::u32string text, float font_size, float x_pos, bool between_chars) {
        float width = 0;
        float current_char_width;
        for (size_t i = 0; i < text.size(); i++) {
            if (text[i] == '\x1B') {
                i++; // Go to color
                if (i >= text.length())
                    break;
                continue;
            }
            width += current_char_width = char_width(text[i], font_size);
            if ((between_chars && x_pos < width - current_char_width / 2) || (!between_chars && x_pos < width))
                return i;
        }
        return text.size();
    }

    std::size_t TextRenderer::text_length(std::u32string text) {
        std::size_t index_escaped = 0;
        for (auto it = text.cbegin(); it != text.cend(); it++) {
            if (*it == '\x1B') {
                it++;
                if (it == text.cend())
                    break;
                continue;
            }
            if (*it == '\n')
                continue;
            index_escaped++;
        }
        return index_escaped;
    }

    Color::Name TextRenderer::parse_color_escape(char32_t c) {
        if (c >= 16)
            return Color::Name::NONE;
        return (Color::Name)c;
    }

    struct TextVertex {
        glm::vec2 position;
        glm::vec2 texcoords;
        glm::vec4 color;

        TextVertex(glm::vec2 position, glm::vec2 texcoords, glm::vec4 color)
            : position(position)
            , texcoords(texcoords)
            , color(color) { }
    };

    Text::Text(UIVector pos, std::string text, Color::Name color, Placement placement, float font_size)
        : Shape(pos, 0_px, color, placement)
        , font_size(font_size)
        , m_shader(ResourceManager::get_shader("text")) {
        m_text = Unicode::utf8_to_utf32(text);
        create_buffers();
        m_dirty = true;
    }

    Text::~Text() {
        delete_buffers();
    }

    void Text::draw(glm::mat4 move) {
        if (m_hidden)
            return;

        if (m_text.empty())
            return;

        if (m_dirty) {
            update_buffers();
            glm::vec2 pos = UIVector::get_relative_position(m_position, m_size, m_parentSize, m_placement);
            m_move_self = glm::mat4(1);
            m_move_self = glm::translate(m_move_self, glm::vec3(pos, 0.0f));
            if (m_rotation != 0)
                m_move_self = glm::rotate(m_move_self, m_rotation, glm::vec3(0, 0, 1));
            m_dirty = false;
        }

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Application::theme().text_renderer().m_texture_atlas);
        m_shader->use();
        m_shader->set_mat4("projection", projection());
        m_shader->set_mat4("move", move);
        m_shader->set_mat4("move_self", m_move_self);
        m_shader->set_int("font_atlas", 0);
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_escaped_text_length * 6, GL_UNSIGNED_INT, 0);
    }

    bool Text::contains(glm::vec2) {
        return false;
    }

    std::string Text::text() {
        return Unicode::utf32_to_utf8(m_text);
    }

    void Text::text(std::string value) {
        m_text = Unicode::utf8_to_utf32(value);
        m_dirty = true;
    }

    std::u32string Text::text_u32() {
        return m_text;
    }

    void Text::text_u32(std::u32string value) {
        m_text = value;
        m_dirty = true;
    }

    void Text::create_buffers() {
        // Create buffers
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);
        // Allocate buffers
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(TextVertex) * m_escaped_text_length * 4, 0, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_escaped_text_length * 6, 0, GL_STATIC_DRAW);
        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)0);
        // uv coordinates
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(2 * sizeof(float)));
        // color
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(4 * sizeof(float)));
    }

    void Text::delete_buffers() {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void Text::update_buffers() {
        // Resize buffers
        if (m_text.size() != m_text_length) {
            m_text_length = m_text.size();
            m_escaped_text_length = TextRenderer::text_length(m_text);
            glBindVertexArray(m_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(TextVertex) * m_escaped_text_length * 4, 0, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_escaped_text_length * 6, 0, GL_STATIC_DRAW);
        }
        // Create data
        TextRenderer& renderer = Application::theme().text_renderer();
        std::vector<TextVertex> vertices;
        std::vector<GLuint> indices;
        vertices.reserve(4 * m_escaped_text_length);
        indices.reserve(6 * m_escaped_text_length);
        if (font_size == 0)
            font_size = renderer.m_font_size;
        float scale = (font_size / renderer.m_font_size);
        float x = 0;
        float y = font_size / 5; // Offet between baseline and bottom
        Color current_color = Application::theme().color(m_color);
        std::size_t index_escaped = 0;
        for (auto it = m_text.cbegin(); it != m_text.cend(); it++, index_escaped++) {
            if (*it == '\n') {
                x = 0;
                y -= renderer.m_theme.line_height();
                continue;
            }

            if (*it == '\x1B') {
                it++; // Go to color
                if (it == m_text.cend())
                    break;
                Color::Name read_color = renderer.parse_color_escape(*it);
                if (read_color != Color::Name::NONE)
                    current_color = Application::theme().color(read_color);
                index_escaped--; // Decrement escaped, because it will be incremented by continue
                continue;
            }

            if (renderer.m_chars.count(*it) == 0)
                renderer.add_char(*it);
            Character ch = renderer.m_chars[*it];
            float bottom_to_origin = (ch.size.y - ch.bearing.y) * scale;
            float xpos = x + ch.bearing.x * scale;
            float ypos = y - bottom_to_origin;
            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            GLuint start_index = vertices.size();
            vertices.emplace_back(glm::vec2(xpos, ypos), glm::vec2(ch.texcoord1.x, ch.texcoord2.y), current_color); // Bottom Left
            vertices.emplace_back(glm::vec2(xpos + w, ypos), glm::vec2(ch.texcoord2.x, ch.texcoord2.y), current_color); // Bottom Right
            vertices.emplace_back(glm::vec2(xpos, ypos + h), glm::vec2(ch.texcoord1.x, ch.texcoord1.y), current_color); // Top Left
            vertices.emplace_back(glm::vec2(xpos + w, ypos + h), glm::vec2(ch.texcoord2.x, ch.texcoord1.y), current_color); // Top Right

            indices.push_back(start_index + 0);
            indices.push_back(start_index + 1);
            indices.push_back(start_index + 2);

            indices.push_back(start_index + 3);
            indices.push_back(start_index + 2);
            indices.push_back(start_index + 1);

            x += (ch.advance >> 6) * scale;
        }
        m_size = glm::vec2(x, renderer.m_theme.line_height() + y);
        // Write to buffers
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TextVertex) * vertices.size(), vertices.data());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint) * indices.size(), indices.data());
    }

}
