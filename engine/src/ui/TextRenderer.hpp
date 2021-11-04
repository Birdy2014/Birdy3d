#pragma once

#include "ui/Shape.hpp"
#include "ui/Units.hpp"
#include <codecvt>
#include <glm/glm.hpp>
#include <locale>
#include <map>
#include <memory>
#include <string>

typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;

namespace Birdy3d {

    class Rectangle;
    class Theme;

    struct Character {
        glm::vec2 texcoord1;
        glm::vec2 texcoord2;
        glm::ivec2 size;
        glm::ivec2 bearing;
        long advance;
    };

    class TextRenderer {
    private:
        // What is this and why is it needed?
        template <class Facet>
        struct DeletableFacet : Facet {
            template <class... Args>
            DeletableFacet(Args&&... args)
                : Facet(std::forward<Args>(args)...) { }
            ~DeletableFacet() { }
        };

    public:
        static std::wstring_convert<DeletableFacet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> converter;

        TextRenderer(Theme&);
        ~TextRenderer();
        void render_text(std::string text, float x, float y, float font_size, Color::Name color = Color::Name::FG, glm::mat4 move = glm::mat4(1), int cursorpos = -1, bool highlight = false, int hlstart = -1, int hlend = -1, Color::Name hlcolor = Color::Name::TEXT_HIGHLIGHT);
        void render_text(std::u32string text, float x, float y, float font_size, Color::Name color = Color::Name::FG, glm::mat4 move = glm::mat4(1), int cursorpos = -1, bool highlight = false, int hlstart = -1, int hlend = -1, Color::Name hlcolor = Color::Name::TEXT_HIGHLIGHT);
        UIVector text_size(std::string text, float font_size = 0);
        UIVector text_size(std::u32string text, float font_size = 0);
        float char_width(char32_t c, float font_size = 0);
        int char_index(std::u32string text, float font_size, float x_pos, bool between_chars = false);

    private:
        friend class Text;

        Theme& m_theme;
        std::map<char32_t, Character> m_chars;
        FT_Library* m_ft;
        FT_Face* m_face;
        std::unique_ptr<Rectangle> m_rect;
        unsigned int m_font_size;
        GLuint m_texture_atlas;
        glm::vec2 m_texture_atlas_size;
        glm::ivec2 m_texture_atlas_current_pos;

        bool add_char(char32_t c);
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

    private:
        std::u32string m_text;
        GLuint m_vao, m_vbo, m_ebo;
        std::shared_ptr<Shader> m_shader;
        std::size_t m_text_length = 0;

        void create_buffers();
        void delete_buffers();
        void update_buffers();
    };

}
