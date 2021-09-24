#include "ui/TextRenderer.hpp"

#include "core/Logger.hpp"
#include "core/RessourceManager.hpp"
#include "render/Shader.hpp"
#include "ui/Rectangle.hpp"
#include "ui/Theme.hpp"
#include <ft2build.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include FT_FREETYPE_H

namespace Birdy3d {

    std::wstring_convert<TextRenderer::DeletableFacet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> TextRenderer::converter;

    TextRenderer::~TextRenderer() {
        FT_Done_Face(*m_face);
        FT_Done_FreeType(*m_ft);
        free(m_face);
        free(m_ft);
    }

    TextRenderer::TextRenderer(Theme& theme)
        : m_theme(theme) {
        std::string path = RessourceManager::get_ressource_path(theme.font, RessourceManager::RessourceType::FONT);
        m_font_size = theme.font_size;
        m_ft = (FT_Library*)malloc(sizeof(FT_Library));
        m_face = (FT_Face*)malloc(sizeof(FT_Face));
        if (FT_Init_FreeType(m_ft))
            Logger::error("freetype: Could not init FreeType Library");
        if (FT_New_Face(*m_ft, path.c_str(), 0, m_face))
            Logger::error("freetype: Failed to load font");
        FT_Set_Pixel_Sizes(*m_face, 0, m_font_size);
        m_rect = std::make_unique<Rectangle>(UIVector(0), UIVector(0), Color::WHITE, Rectangle::Type::TEXT);

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
            glm::vec2((float)m_texture_atlas_current_pos.x / m_texture_atlas_size.x, (float)m_texture_atlas_current_pos.y / m_texture_atlas_size.y),
            glm::vec2((float)(m_texture_atlas_current_pos.x + (*m_face)->glyph->bitmap.width) / (float)m_texture_atlas_size.x, (float)m_texture_atlas_current_pos.y / m_texture_atlas_size.y + (*m_face)->glyph->bitmap.rows / m_texture_atlas_size.y),
            glm::ivec2((*m_face)->glyph->bitmap.width, (*m_face)->glyph->bitmap.rows),
            glm::ivec2((*m_face)->glyph->bitmap_left, (*m_face)->glyph->bitmap_top),
            (*m_face)->glyph->advance.x
        };
        m_chars.insert(std::pair<char, Character>(c, character));
        m_texture_atlas_current_pos.x += (*m_face)->glyph->bitmap.width + 1;
        return true;
    }

    void TextRenderer::render_text(std::string text, float x, float y, float font_size, Color color, glm::mat4 move, int cursorpos, bool highlight, int hlstart, int hlend, Color hlcolor) {
        std::u32string converted = converter.from_bytes(text);
        render_text(converted, x, y, font_size, color, move, cursorpos, highlight, hlstart, hlend, hlcolor);
    }

    void TextRenderer::render_text(std::u32string text, float x, float y, float font_size, Color color, glm::mat4 move, int cursorpos, bool highlight, int hlstart, int hlend, Color hlcolor) {
        if (hlstart > hlend) {
            std::swap(hlstart, hlend);
            hlend--;
        }

        float initial_x = x;

        m_rect->type = Rectangle::TEXT;
        m_rect->texture(m_texture_atlas);

        y += font_size / 5; // Offet between baseline and bottom

        float hlstart_x = x;
        float hlend_x = x;
        float scale = (font_size / m_font_size);
        char16_t c;
        for (int i = 0; i <= (int)text.length(); i++) {
            if (i < (int)text.length())
                c = text[i];
            else
                c = ' ';

            if (c == '\n') {
                x = initial_x;
                y -= m_theme.line_height;
                continue;
            }

            if (i == hlstart)
                hlstart_x = x;
            if (i == hlend + 1)
                hlend_x = x;

            if (m_chars.count(c) == 0) {
                add_char(c);
            }
            Character ch = m_chars[c];
            float bottom_to_origin = (ch.size.y - ch.bearing.y) * scale;
            float xpos = x + ch.bearing.x * scale;
            float ypos = y;
            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            m_rect->position(UIVector(xpos, ypos - bottom_to_origin));
            m_rect->color(color);

            m_rect->size(UIVector(w, h));
            m_rect->texcoords(ch.texcoord1, ch.texcoord2);

            m_rect->draw(move);

            if (i == cursorpos) {
                m_rect->type = Rectangle::FILLED;
                m_rect->position(UIVector(xpos - 2, ypos - font_size / 5));
                m_rect->size(UIVector(2, font_size));
                m_rect->draw(move);

                m_rect->type = Rectangle::TEXT;
            }

            if (hlend > (int)text.length())
                hlend_x = x;

            x += (ch.advance >> 6) * scale;
        }

        if (highlight && hlstart_x != hlend_x) {
            m_rect->type = Rectangle::FILLED;
            m_rect->color(hlcolor);
            m_rect->position(UIVector(hlstart_x, y - font_size / 5));
            m_rect->size(UIVector(hlend_x - hlstart_x, font_size));
            m_rect->draw(move);
        }
    }

    UIVector TextRenderer::text_size(std::string text, float font_size) {
        std::u32string converted = converter.from_bytes(text);
        return text_size(converted, font_size);
    }

    UIVector TextRenderer::text_size(std::u32string text, float font_size) {
        float scale = (font_size / m_font_size);
        UIVector size(0_px, font_size);
        float current_x = 0;
        for (std::u32string::const_iterator c = text.begin(); c != text.end(); c++) {
            if (*c == '\n') {
                size.x = std::max(size.x.to_pixels(), current_x);
                size.y += m_theme.line_height;
                current_x = 0;
                continue;
            }
            if (m_chars.count(*c) == 0)
                add_char(*c);
            Character ch = m_chars[*c];
            current_x += (ch.advance >> 6) * scale;
        }
        size.x = std::max(size.x.to_pixels(), current_x);
        return size;
    }

    float TextRenderer::char_width(char32_t c, float font_size) {
        if (m_chars.count(c) == 0)
            add_char(c);
        Character ch = m_chars[c];
        return (ch.advance >> 6) * (font_size / m_font_size);
    }

    int TextRenderer::char_index(std::u32string text, float font_size, float x_pos, bool between_chars) {
        float width = 0;
        float current_char_width;
        for (size_t i = 0; i < text.size(); i++) {
            width += current_char_width = char_width(text[i], font_size);
            if ((between_chars && x_pos < width - current_char_width / 2) || (!between_chars && x_pos < width))
                return i;
        }
        return text.size();
    }

    Text::Text(UIVector pos, float font_size, std::string text, Color color, Placement placement, TextRenderer* renderer)
        : Shape(pos, 0_px, color, placement)
        , font_size(font_size)
        , renderer(renderer)
        , m_shader(RessourceManager::get_shader("ui")) {
        m_text = TextRenderer::converter.from_bytes(text);
        create_buffers();
        m_dirty = true;
    }

    void Text::draw(glm::mat4 move) {
        if (m_hidden)
            return;

        if (m_dirty) {
            delete_buffers();
            create_buffers();
            glm::vec2 pos = Utils::get_relative_position(m_position, m_size, m_parentSize, m_placement);
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
        glBindTexture(GL_TEXTURE_2D, renderer->m_texture_atlas);
        m_shader->use();
        m_shader->set_int("type", Shape::Type::TEXT);
        m_shader->set_mat4("projection", projection());
        m_shader->set_mat4("move", move);
        m_shader->set_mat4("move_self", m_move_self);
        m_shader->set_vec4("color", m_color);
        m_shader->set_int("rectTexture", 0);
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_text.size() * 6, GL_UNSIGNED_INT, 0);
    }

    bool Text::contains(glm::vec2) {
        return false;
    }

    std::string Text::text() {
        return TextRenderer::converter.to_bytes(m_text);
    }

    void Text::text(std::string value) {
        m_text = TextRenderer::converter.from_bytes(value);
        m_dirty = true;
    }

    void Text::create_buffers() {
        // Create buffers
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);
        // Create data
        std::vector<UIVertex> vertices;
        std::vector<GLuint> indices;
        vertices.reserve(4 * m_text.size());
        float scale = (font_size / renderer->m_font_size);
        float x = 0;
        float y = font_size / 5; // Offet between baseline and bottom
        for (char32_t c : m_text) {
            if (c == '\n') {
                x = 0;
                y -= renderer->m_theme.line_height;
                continue;
            }

            if (renderer->m_chars.count(c) == 0)
                renderer->add_char(c);
            Character ch = renderer->m_chars[c];
            float bottom_to_origin = (ch.size.y - ch.bearing.y) * scale;
            float xpos = x + ch.bearing.x * scale;
            float ypos = y - bottom_to_origin;
            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            GLuint start_index = vertices.size();
            vertices.emplace_back(glm::vec2(xpos, ypos), glm::vec2(ch.texcoord1.x, ch.texcoord2.y)); // Bottom Left
            vertices.emplace_back(glm::vec2(xpos + w, ypos), glm::vec2(ch.texcoord2.x, ch.texcoord2.y)); // Bottom Right
            vertices.emplace_back(glm::vec2(xpos, ypos + h), glm::vec2(ch.texcoord1.x, ch.texcoord1.y)); // Top Left
            vertices.emplace_back(glm::vec2(xpos + w, ypos + h), glm::vec2(ch.texcoord2.x, ch.texcoord1.y)); // Top Right

            indices.push_back(start_index + 0);
            indices.push_back(start_index + 1);
            indices.push_back(start_index + 2);

            indices.push_back(start_index + 3);
            indices.push_back(start_index + 2);
            indices.push_back(start_index + 1);

            x += (ch.advance >> 6) * scale;
        }
        m_size = glm::vec2(x, renderer->m_theme.line_height + y);
        // Write to buffers
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        // uv coordinates
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    void Text::delete_buffers() {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

}
