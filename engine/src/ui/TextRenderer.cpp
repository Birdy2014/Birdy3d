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

namespace Birdy3d::ui {

    TextRenderer::~TextRenderer() {
        FT_Done_Face(*m_face);
        FT_Done_FreeType(*m_ft);
        free(m_face);
        free(m_ft);
    }

    TextRenderer::TextRenderer(Theme& theme)
        : m_theme(theme) {
        std::string path = core::ResourceManager::get_resource_path(theme.font(), core::ResourceType::FONT);
        m_font_size = theme.font_size();
        m_ft = (FT_Library*)malloc(sizeof(FT_Library));
        m_face = (FT_Face*)malloc(sizeof(FT_Face));
        if (FT_Init_FreeType(m_ft))
            core::Logger::critical("freetype: Could not init FreeType Library");
        if (FT_New_Face(*m_ft, path.c_str(), 0, m_face))
            throw std::runtime_error("freetype: Failed to load font");
        FT_Set_Pixel_Sizes(*m_face, 0, m_font_size);
        m_rect = std::make_unique<Rectangle>(UIVector(0), UIVector(0), utils::Color::Name::FG, Rectangle::Type::FILLED);
        m_text = std::make_unique<Text>(0_px, "", utils::Color::Name::FG, Placement::TOP_LEFT, m_font_size);

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
            core::Logger::warn("freetype: Failed to load glyph '{}'", (unsigned)c);
            return false;
        }
        if (m_texture_atlas_current_pos.x + (*m_face)->glyph->bitmap.width > m_texture_atlas_size.x) {
            if (m_texture_atlas_current_pos.y + m_font_size > m_texture_atlas_size.y) {
                core::Logger::warn("Failed to load glyph '{}': no space left in font atlas", (unsigned)c);
                return false;
            }
            m_texture_atlas_current_pos.x = 0;
            m_texture_atlas_current_pos.y += m_texture_atlas_current_line_height;
            m_texture_atlas_current_line_height = 0;
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
        if (m_texture_atlas_current_line_height < character.size.y)
            m_texture_atlas_current_line_height = character.size.y;
        return true;
    }

    void TextRenderer::render_text(std::string text, float x, float y, float font_size, utils::Color::Name color, glm::mat4 move, bool cursor, std::size_t cursorpos, bool highlight, std::size_t hlstart, std::size_t hlend, utils::Color::Name hlcolor) {
        std::u32string converted = utils::Unicode::utf8_to_utf32(text);
        render_text(converted, x, y, font_size, color, move, cursor, cursorpos, highlight, hlstart, hlend, hlcolor);
    }

    void TextRenderer::render_text(std::u32string text, float x, float y, float font_size, utils::Color::Name color, glm::mat4 move, bool cursor, std::size_t cursorpos, bool highlight, std::size_t hlstart, std::size_t hlend, utils::Color::Name hlcolor) {
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
            float ypos = y + size.y - m_theme.line_height();

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
        std::u32string converted = utils::Unicode::utf8_to_utf32(text);
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

    utils::Color::Name TextRenderer::parse_color_escape(char32_t c) {
        // NONE must be the last element; use reflection when available.
        if (c >= (int)utils::Color::Name::NONE)
            return utils::Color::Name::NONE;
        return static_cast<utils::Color::Name>(c);
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

    std::unique_ptr<Rectangle> Text::m_cursor_rect;

    Text::Text(UIVector pos, std::string text, utils::Color::Name color, Placement placement, float font_size)
        : Shape(pos, 0_px, color, placement)
        , font_size(font_size)
        , m_shader(core::ResourceManager::get_shader("text.glsl")) {
        if (!m_cursor_rect)
            m_cursor_rect = std::make_unique<Rectangle>(0_px, 0_px, utils::Color::Name::FG);
        m_text = utils::Unicode::utf8_to_utf32(text);
        create_buffers();
        m_dirty = true;
    }

    Text::~Text() {
        delete_buffers();
    }

    void Text::draw(glm::mat4 move) {
        if (m_hidden)
            return;

        if (m_text.empty()) {
            if (cursor_visible)
                draw_cursor(move);
            return;
        }

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
        glBindTexture(GL_TEXTURE_2D, core::Application::theme().text_renderer().m_texture_atlas);
        m_shader->use();
        m_shader->set_mat4("projection", projection());
        m_shader->set_mat4("move", move);
        m_shader->set_mat4("move_self", m_move_self);
        m_shader->set_int("font_atlas", 0);
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_escaped_text_length * 6, GL_UNSIGNED_INT, 0);

        if (cursor_visible)
            draw_cursor(move);

        if (highlight_visible)
            draw_highlight(move);
    }

    bool Text::contains(glm::vec2) {
        return false;
    }

    std::string Text::text() const {
        return utils::Unicode::utf32_to_utf8(m_text);
    }

    void Text::text(std::string value) {
        m_text = utils::Unicode::utf8_to_utf32(value);
        m_dirty = true;
    }

    std::u32string Text::text_u32() const {
        return m_text;
    }

    void Text::text_u32(std::u32string value) {
        m_text = value;
        m_dirty = true;
    }

    void Text::append(std::string value) {
        if (value.empty())
            return;
        append(utils::Unicode::utf8_to_utf32(value));
    }

    void Text::append(std::u32string value) {
        if (value.empty())
            return;
        m_text += value;
    }

    void Text::clear() {
        m_text.clear();
        m_dirty = true;
    }

    void Text::create_buffers() {
        // Create buffers
        glCreateVertexArrays(1, &m_vao);
        glCreateBuffers(1, &m_vbo);
        glCreateBuffers(1, &m_ebo);
        // Allocate buffers
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glNamedBufferData(m_vbo, sizeof(TextVertex) * m_buffer_char_capacity * 4, 0, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glNamedBufferData(m_ebo, sizeof(GLuint) * m_buffer_char_capacity * 6, 0, GL_DYNAMIC_DRAW);
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

    void Text::resize_buffers_if_needed() {
        auto const increase_size = 10;

        auto const new_text_length = TextRenderer::text_length(m_text);
        m_escaped_text_length = new_text_length;

        if (new_text_length > m_buffer_char_capacity) {
            m_buffer_char_capacity = new_text_length + increase_size;
            glBindVertexArray(m_vao);
            glNamedBufferData(m_vbo, sizeof(TextVertex) * m_buffer_char_capacity * 4, 0, GL_DYNAMIC_DRAW);
            glNamedBufferData(m_ebo, sizeof(GLuint) * m_buffer_char_capacity * 6, 0, GL_DYNAMIC_DRAW);
        }
    }

    void Text::update_buffers() {
        resize_buffers_if_needed();

        // Create data
        TextRenderer& renderer = core::Application::theme().text_renderer();
        std::vector<TextVertex> vertices;
        std::vector<GLuint> indices;
        vertices.reserve(4 * m_buffer_char_capacity);
        indices.reserve(6 * m_buffer_char_capacity);
        if (font_size == 0)
            font_size = renderer.m_font_size;
        float scale = (font_size / renderer.m_font_size);
        float max_x = 0;
        float x = 0;
        float y = 0;
        utils::Color current_color = core::Application::theme().color(m_color);
        std::size_t index_escaped = 0;
        for (auto it = m_text.cbegin(); it != m_text.cend(); it++, index_escaped++) {
            if (*it == '\n') {
                if (max_x < x)
                    max_x = x;
                x = 0;
                y += renderer.m_theme.line_height();
                continue;
            }

            if (*it == '\x1B') {
                it++; // Go to color
                if (it == m_text.cend())
                    break;
                utils::Color::Name read_color = renderer.parse_color_escape(*it);
                if (read_color != utils::Color::Name::NONE)
                    current_color = core::Application::theme().color(read_color);
                index_escaped--; // Decrement escaped, because it will be incremented by continue
                continue;
            }

            if (renderer.m_chars.count(*it) == 0)
                renderer.add_char(*it);
            Character ch = renderer.m_chars[*it];

            float xpos = x + ch.bearing.x * scale;

            float max_font_top_to_origin = font_size * (4.0f / 5.0f);
            float ypos = y + max_font_top_to_origin - ch.bearing.y;

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            GLuint start_index = vertices.size();
            vertices.emplace_back(glm::vec2(xpos, ypos + h), glm::vec2(ch.texcoord1.x, ch.texcoord2.y), current_color); // Bottom Left
            vertices.emplace_back(glm::vec2(xpos + w, ypos + h), glm::vec2(ch.texcoord2.x, ch.texcoord2.y), current_color); // Bottom Right
            vertices.emplace_back(glm::vec2(xpos, ypos), glm::vec2(ch.texcoord1.x, ch.texcoord1.y), current_color); // Top Left
            vertices.emplace_back(glm::vec2(xpos + w, ypos), glm::vec2(ch.texcoord2.x, ch.texcoord1.y), current_color); // Top Right

            indices.push_back(start_index + 0);
            indices.push_back(start_index + 1);
            indices.push_back(start_index + 2);

            indices.push_back(start_index + 3);
            indices.push_back(start_index + 2);
            indices.push_back(start_index + 1);

            x += (ch.advance >> 6) * scale;
        }
        if (max_x < x)
            max_x = x;
        m_size = glm::vec2(max_x, renderer.m_theme.line_height() + y);
        // Write to buffers
        glBindVertexArray(m_vao);
        glNamedBufferSubData(m_vbo, 0, sizeof(TextVertex) * vertices.size(), vertices.data());
        glNamedBufferSubData(m_ebo, 0, sizeof(GLuint) * indices.size(), indices.data());
    }

    void Text::draw_cursor(glm::mat4 move) {
        Theme& theme = core::Application::theme();
        TextRenderer& renderer = theme.text_renderer();

        float scale = font_size > 0 ? font_size / theme.text_renderer().m_font_size : 1;
        glm::vec2 cursor_pixel_pos(0, font_size);
        for (std::size_t i = 0; i < m_text.length() && i < cursor_pos; i++) {
            char32_t c = m_text[i];
            if (c == '\n') {
                cursor_pixel_pos.x = 0;
                cursor_pixel_pos.y += theme.line_height();
                continue;
            }
            if (c == '\x1B') {
                i++; // Go to color
                if (i >= m_text.length() || i >= cursor_pos)
                    break;
                continue;
            }
            if (renderer.m_chars.count(c) == 0)
                renderer.add_char(c);
            Character ch = renderer.m_chars[c];
            cursor_pixel_pos.x += (ch.advance >> 6) * scale;
        }

        float xpos = m_position.x + cursor_pixel_pos.x - 1;
        float ypos = m_position.y + cursor_pixel_pos.y - theme.line_height();

        m_cursor_rect->type = Rectangle::FILLED;
        m_cursor_rect->position(UIVector(xpos, ypos));
        m_cursor_rect->size(UIVector(2, font_size));
        m_cursor_rect->color(m_color);
        m_cursor_rect->draw(move);
    }

    void Text::draw_highlight(glm::mat4 move) {
        std::size_t hl_start = highlight_start;
        std::size_t hl_end = highlight_end;
        if (hl_start > hl_end) {
            std::swap(hl_start, hl_end);
            --hl_end;
        }

        Theme& theme = core::Application::theme();
        float scale = font_size > 0 ? font_size / theme.text_renderer().m_font_size : 1;
        glm::vec2 current_pos = m_position;
        glm::vec2 start_pos;
        bool started_highlight = false;
        std::size_t index_escaped = 0;
        for (auto it = m_text.cbegin(); it != m_text.cend(); it++, index_escaped++) {
            if (index_escaped > hl_end)
                break;

            if (*it == '\x1B') {
                it++;
                if (it == m_text.cend())
                    break;
                index_escaped--;
                continue;
            }

            if (index_escaped == hl_start) {
                start_pos = current_pos;
                started_highlight = true;
            }

            if (*it == '\n' || index_escaped == hl_end + 1) {
                if (started_highlight) {
                    m_cursor_rect->type = Rectangle::FILLED;
                    m_cursor_rect->position(start_pos);
                    m_cursor_rect->size(UIVector(current_pos.x - start_pos.x, font_size));
                    m_cursor_rect->color(utils::Color::Name::TEXT_HIGHLIGHT);
                    m_cursor_rect->draw(move);
                }
                if (index_escaped >= hl_end)
                    return;
                current_pos.x = 0;
                current_pos.y += theme.line_height();
                start_pos = current_pos;
                continue;
            }

            if (theme.text_renderer().m_chars.count(*it) == 0)
                theme.text_renderer().add_char(*it);
            Character ch = theme.text_renderer().m_chars[*it];
            current_pos.x += (ch.advance >> 6) * scale;
        }

        if (started_highlight) {
            m_cursor_rect->type = Rectangle::FILLED;
            m_cursor_rect->position(start_pos);
            m_cursor_rect->size(UIVector(current_pos.x - start_pos.x, font_size));
            m_cursor_rect->color(utils::Color::Name::TEXT_HIGHLIGHT);
            m_cursor_rect->draw(move);
        }
    }

}
