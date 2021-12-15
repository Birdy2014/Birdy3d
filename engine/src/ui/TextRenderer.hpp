#pragma once

#include "ui/Shape.hpp"
#include "ui/Units.hpp"
#include <glm/glm.hpp>
#include <limits>
#include <map>
#include <memory>
#include <string>

typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;

namespace Birdy3d::ui {

    class Rectangle;
    class Text;
    class Theme;

    struct Character {
        glm::vec2 texcoord1;
        glm::vec2 texcoord2;
        glm::ivec2 size;
        glm::ivec2 bearing;
        long advance;
    };

    class TextRenderer {
    public:
        TextRenderer(Theme&);
        ~TextRenderer();
        void render_text(std::string text, float x, float y, float font_size, utils::Color::Name color = utils::Color::Name::FG, glm::mat4 move = glm::mat4(1), bool cursor = false, std::size_t cursorpos = 0, bool highlight = false, std::size_t hlstart = -1, std::size_t hlend = -1, utils::Color::Name hlcolor = utils::Color::Name::TEXT_HIGHLIGHT);
        void render_text(std::u32string text, float x, float y, float font_size, utils::Color::Name color = utils::Color::Name::FG, glm::mat4 move = glm::mat4(1), bool cursor = false, std::size_t cursorpos = 0, bool highlight = false, std::size_t hlstart = -1, std::size_t hlend = -1, utils::Color::Name hlcolor = utils::Color::Name::TEXT_HIGHLIGHT);
        UIVector text_size(std::string text, float font_size = 0, std::size_t n = std::numeric_limits<std::size_t>::max());
        UIVector text_size(std::u32string text, float font_size = 0, std::size_t n = std::numeric_limits<std::size_t>::max());
        float char_width(char32_t c, float font_size = 0);
        int char_index(std::u32string text, float font_size, float x_pos, bool between_chars = false);
        static std::size_t text_length(std::u32string);

    private:
        friend class Text;

        Theme& m_theme;
        std::map<char32_t, Character> m_chars;
        FT_Library* m_ft;
        FT_Face* m_face;
        std::unique_ptr<Rectangle> m_rect;
        std::unique_ptr<Text> m_text;
        unsigned int m_font_size;
        GLuint m_texture_atlas;
        glm::vec2 m_texture_atlas_size;
        glm::ivec2 m_texture_atlas_current_pos;

        bool add_char(char32_t c);
        utils::Color::Name parse_color_escape(char32_t);
    };

    class Text : public Shape {
    public:
        float font_size;
        bool cursor_visible = false;
        std::size_t cursor_pos = 0;
        bool highlight_visible = false;
        std::size_t highlight_start = 0;
        std::size_t highlight_end = 0;

        Text(UIVector pos, std::string text, utils::Color::Name color, Placement placement, float font_size = 0);
        ~Text();
        void draw(glm::mat4 move) override;
        bool contains(glm::vec2 point) override;
        std::string text() const;
        void text(std::string value);
        std::u32string text_u32() const;
        void text_u32(std::u32string value);
        void append(std::string);
        void append(std::u32string);
        void clear();

        void operator=(std::string value) { text(value); }
        void operator=(std::u32string value) { text_u32(value); }
        operator std::string() const { return text(); }
        operator std::u32string() const { return text_u32(); }
        void operator+=(std::string value) { append(value); }
        void operator+=(std::u32string value) { append(value); }
        bool operator==(std::string value) const { return text() == value; }
        bool operator==(std::u32string value) const { return text_u32() == value; }
        std::u32string::iterator begin() { return m_text.begin(); }
        std::u32string::iterator end() { return m_text.end(); }
        std::size_t length() const { return m_text.length(); }
        void insert(std::size_t index, char32_t* value) { m_text.insert(index, value); }
        void erase(std::size_t index) { m_text.erase(m_text.begin() + index, m_text.begin() + index + 1); }
        void erase(std::size_t start_index, std::size_t end_index) { m_text.erase(m_text.begin() + start_index, m_text.begin() + end_index); }

    private:
        std::u32string m_text;
        GLuint m_vao, m_vbo, m_ebo;
        std::shared_ptr<render::Shader> m_shader;
        static std::unique_ptr<Rectangle> m_cursor_rect;

        // Buffer size is smaller than text_length if the text contains escape characters.
        std::size_t m_text_length = 0;
        std::size_t m_escaped_text_length = 0;

        void create_buffers();
        void delete_buffers();
        void update_buffers();
        void draw_cursor(glm::mat4 move);
        void draw_highlight(glm::mat4 move);
    };

}
