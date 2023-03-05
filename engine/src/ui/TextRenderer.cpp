#include "ui/TextRenderer.hpp"

#include "core/Logger.hpp"
#include "core/ResourceManager.hpp"
#include "render/Shader.hpp"
#include "ui/Theme.hpp"
#include "utils/Unicode.hpp"
#include <ft2build.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include FT_FREETYPE_H

namespace Birdy3d::ui {

    TextRenderer::~TextRenderer()
    {
        FT_Done_Face(*m_face);
        FT_Done_FreeType(*m_ft);
        free(m_face);
        free(m_ft);
        delete_buffers();
    }

    TextRenderer::TextRenderer(Theme& theme)
        : m_theme(theme)
    {
        std::string path = core::ResourceManager::get_resource_path(theme.font(), core::ResourceType::FONT);
        m_font_size = theme.font_size();
        m_ft = (FT_Library*)malloc(sizeof(FT_Library));
        m_face = (FT_Face*)malloc(sizeof(FT_Face));
        if (FT_Init_FreeType(m_ft))
            core::Logger::critical("freetype: Could not init FreeType Library");
        if (FT_New_Face(*m_ft, path.c_str(), 0, m_face))
            throw std::runtime_error("freetype: Failed to load font");
        FT_Set_Pixel_Sizes(*m_face, 0, m_font_size);

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

        m_shader = core::ResourceManager::get_shader("text.glsl");
        create_buffers();
    }

    bool TextRenderer::add_char(char32_t c)
    {
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
            (*m_face)->glyph->advance.x};
        m_chars.insert(std::pair<char, Character>(c, character));
        m_texture_atlas_current_pos.x += (*m_face)->glyph->bitmap.width + 1;
        if (m_texture_atlas_current_line_height < character.size.y)
            m_texture_atlas_current_line_height = character.size.y;
        return true;
    }

    void TextRenderer::render_text(std::u32string text, int x, int y, int font_size)
    {
        if (text.empty())
            return;

        // Render text
        auto position = glm::ivec2(x, y);

        {
            auto const increase_size = 10;

            auto const new_text_length = TextRenderer::text_length(text);
            m_escaped_text_length = new_text_length;

            if (new_text_length > m_buffer_char_capacity) {
                m_buffer_char_capacity = new_text_length + increase_size;
                glBindVertexArray(m_vao);
                glNamedBufferData(m_vbo, sizeof(TextVertex) * m_buffer_char_capacity * 4, 0, GL_DYNAMIC_DRAW);
                glNamedBufferData(m_ebo, sizeof(GLuint) * m_buffer_char_capacity * 6, 0, GL_DYNAMIC_DRAW);
            }
        }

        // Create data
        {
            TextRenderer& renderer = core::Application::theme().text_renderer();
            m_vertices.clear();
            m_indices.clear();
            m_vertices.reserve(4 * m_buffer_char_capacity);
            m_indices.reserve(6 * m_buffer_char_capacity);
            if (font_size == 0)
                font_size = renderer.m_font_size;
            float scale = (font_size / renderer.m_font_size);
            float max_x = 0;
            float x = 0;
            float y = 0;
            utils::Color current_color = core::Application::theme().color(utils::Color::Name::FG);
            std::size_t index_escaped = 0;
            for (auto it = text.cbegin(); it != text.cend(); it++, index_escaped++) {
                if (*it == '\n') {
                    if (max_x < x)
                        max_x = x;
                    x = 0;
                    y += renderer.m_theme.line_height();
                    continue;
                }

                if (*it == '\x1B') {
                    it++; // Go to color
                    if (it == text.cend())
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

                GLuint start_index = m_vertices.size();
                m_vertices.emplace_back(glm::vec2(xpos, ypos + h), glm::vec2(ch.texcoord1.x, ch.texcoord2.y), current_color); // Bottom Left
                m_vertices.emplace_back(glm::vec2(xpos + w, ypos + h), glm::vec2(ch.texcoord2.x, ch.texcoord2.y), current_color); // Bottom Right
                m_vertices.emplace_back(glm::vec2(xpos, ypos), glm::vec2(ch.texcoord1.x, ch.texcoord1.y), current_color); // Top Left
                m_vertices.emplace_back(glm::vec2(xpos + w, ypos), glm::vec2(ch.texcoord2.x, ch.texcoord1.y), current_color); // Top Right

                m_indices.push_back(start_index + 0);
                m_indices.push_back(start_index + 1);
                m_indices.push_back(start_index + 2);

                m_indices.push_back(start_index + 3);
                m_indices.push_back(start_index + 2);
                m_indices.push_back(start_index + 1);

                x += (ch.advance >> 6) * scale;
            }
            if (max_x < x)
                max_x = x;

            // Write to buffers
            glBindVertexArray(m_vao);
            glNamedBufferSubData(m_vbo, 0, sizeof(TextVertex) * m_vertices.size(), m_vertices.data());
            glNamedBufferSubData(m_ebo, 0, sizeof(GLuint) * m_indices.size(), m_indices.data());
        }

        auto move = glm::translate(glm::mat4(1), glm::vec3(position, 0.0f));

        auto viewport = core::Application::get_viewport_size();
        auto projection_matrix = glm::ortho(0.0f, viewport.x - 1.0f, viewport.y - 1.0f, 0.0f);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, core::Application::theme().text_renderer().m_texture_atlas);
        m_shader->use();
        m_shader->set_mat4("projection", projection_matrix);
        m_shader->set_mat4("move", move);
        m_shader->set_int("font_atlas", 0);
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_escaped_text_length * 6, GL_UNSIGNED_INT, 0);
    }

    Size TextRenderer::text_size(std::string text, int font_size, std::size_t n)
    {
        std::u32string converted = utils::Unicode::utf8_to_utf32(text);
        return text_size(converted, font_size, n);
    }

    Size TextRenderer::text_size(std::u32string text, int font_size, std::size_t n)
    {
        if (!font_size)
            font_size = m_font_size;
        float scale = (font_size / m_font_size);
        auto size = Size::make_pixels(0, font_size);
        int current_x = 0;
        for (std::size_t i = 0; i < text.length() && i < n; i++) {
            char32_t c = text[i];
            if (c == '\n') {
                size.x = Dimension::make_pixels(std::max(size.x.to_pixels(), current_x));
                size.y += Dimension::make_pixels(m_theme.line_height());
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
        size.x = Dimension::make_pixels(std::max(size.x.to_pixels(), current_x));
        return size;
    }

    float TextRenderer::char_width(char32_t c, int font_size)
    {
        if (!font_size)
            font_size = m_font_size;
        if (m_chars.count(c) == 0)
            add_char(c);
        Character ch = m_chars[c];
        return (ch.advance >> 6) * (font_size / m_font_size);
    }

    std::size_t TextRenderer::text_length(std::u32string text)
    {
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

    utils::Color::Name TextRenderer::parse_color_escape(char32_t c)
    {
        // NONE must be the last element; use reflection when available.
        if (c >= (int)utils::Color::Name::NONE)
            return utils::Color::Name::NONE;
        return static_cast<utils::Color::Name>(c);
    }

    void TextRenderer::create_buffers()
    {
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

    void TextRenderer::delete_buffers()
    {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

}
