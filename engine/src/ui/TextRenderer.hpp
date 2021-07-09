#pragma once

#include "ui/Shape.hpp"
#include "ui/Utils.hpp"
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
    class TextRenderer;

    struct Character {
        unsigned int textureID;
        glm::ivec2 size;
        glm::ivec2 bearing;
        long advance;
    };

    class TextRenderer {
    private:
        // What is this and why is it needed?
        template <class Facet>
        struct deletable_facet : Facet {
            template <class... Args>
            deletable_facet(Args&&... args)
                : Facet(std::forward<Args>(args)...) { }
            ~deletable_facet() { }
        };

    public:
        std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> converter;

        TextRenderer(std::string path, unsigned int fontSize);
        ~TextRenderer();
        void renderText(std::string text, float x, float y, float fontSize, Color color);
        void renderText(std::string text, float x, float y, float fontSize, Color color, glm::mat4 move, int cursorpos = -1, bool highlight = false, int hlstart = -1, int hlend = -1, Color hlcolor = Color::BLACK);
        void renderText(std::u32string text, float x, float y, float fontSize, Color color, glm::mat4 move, int cursorpos = -1, bool highlight = false, int hlstart = -1, int hlend = -1, Color hlcolor = Color::BLACK);
        UIVector textSize(std::string text, float fontSize);
        UIVector textSize(std::u32string text, float fontSize);
        float charWidth(char32_t c, float fontSize);
        int char_index(std::u32string text, float font_size, float x_pos, bool between_chars = false);

    private:
        std::map<char, Character> m_chars;
        FT_Library* m_ft;
        FT_Face* m_face;
        std::unique_ptr<Rectangle> m_rect;
        unsigned int m_fontSize;

        bool addChar(char32_t c);
    };

    class Text : public Shape {
    public:
        float fontSize;
        std::string text;
        TextRenderer* renderer;

        Text(UIVector pos, float fontSize, std::string text, Color color, Placement placement, TextRenderer* renderer)
            : Shape(pos, 0_px, color, placement)
            , fontSize(fontSize)
            , text(text)
            , renderer(renderer) { }
        void draw(glm::mat4 move) override;
        bool contains(glm::vec2 point) override;

    private:
        glm::vec2 m_relativePos;
    };

}
