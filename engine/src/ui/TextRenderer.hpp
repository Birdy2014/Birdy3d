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

namespace Birdy3d {

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
        void render_text(std::string text, float x, float y, float font_size, Color::Name color = Color::Name::FG, glm::mat4 move = glm::mat4(1), bool cursor = false, std::size_t cursorpos = 0, bool highlight = false, std::size_t hlstart = -1, std::size_t hlend = -1, Color::Name hlcolor = Color::Name::TEXT_HIGHLIGHT);
        void render_text(std::u32string text, float x, float y, float font_size, Color::Name color = Color::Name::FG, glm::mat4 move = glm::mat4(1), bool cursor = false, std::size_t cursorpos = 0, bool highlight = false, std::size_t hlstart = -1, std::size_t hlend = -1, Color::Name hlcolor = Color::Name::TEXT_HIGHLIGHT);
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
        Color::Name parse_color_escape(char32_t);
    };

    class Text : public Shape {
    public:
        float font_size;

        Text(UIVector pos, std::string text, Color::Name color, Placement placement, float font_size = 0);
        ~Text();
        void draw(glm::mat4 move) override;
        bool contains(glm::vec2 point) override;
        std::string text();
        void text(std::string value);
        std::u32string text_u32();
        void text_u32(std::u32string value);

    private:
        std::u32string m_text;
        GLuint m_vao, m_vbo, m_ebo;
        std::shared_ptr<Shader> m_shader;

        // Buffer size is smaller than text_length if the text contains escape characters.
        std::size_t m_text_length = 0;
        std::size_t m_escaped_text_length = 0;

        void create_buffers();
        void delete_buffers();
        void update_buffers();
    };

}
