#pragma once

#include "ui/Shape.hpp"
#include "ui/Utils.hpp"
#include <codecvt>
#include <ft2build.h>
#include <glm/glm.hpp>
#include <locale>
#include <map>
#include <string>
#include FT_FREETYPE_H

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
        void renderText(std::string text, float x, float y, float fontSize, Color color, glm::mat4 move, int cursorpos = -1, int hlstart = -1, int hlend = -1, Color hlcolor = Color::BLACK);
        void renderText(std::u32string text, float x, float y, float fontSize, Color color, glm::mat4 move, int cursorpos = -1, int hlstart = -1, int hlend = -1, Color hlcolor = Color::BLACK);
        UIVector textSize(std::string text, float fontSize);
        UIVector textSize(std::u32string text, float fontSize);
        float charWidth(char32_t c, float fontSize);

    private:
        std::map<char, Character> chars;
        FT_Library ft;
        FT_Face face;
        Rectangle* rect;
        unsigned int fontSize;

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
        glm::vec2 relativePos;
    };

}
