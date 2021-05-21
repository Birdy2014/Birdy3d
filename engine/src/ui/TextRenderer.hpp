#pragma once

#include "ui/Shape.hpp"
#include "ui/Utils.hpp"
#include <ft2build.h>
#include <glm/glm.hpp>
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
    public:
        TextRenderer(std::string path, unsigned int fontSize);
        ~TextRenderer();
        void renderText(std::string text, float x, float y, float fontSize, Color color);
        void renderText(std::string text, float x, float y, float fontSize, Color color, glm::mat4 move, int cursorpos = -1, int hlstart = -1, int hlend = -1, Color hlcolor = Color::BLACK);
        UIVector textSize(std::string text, float fontSize);
        float charWidth(char c, float fontSize);

    private:
        std::map<char, Character> chars;
        FT_Library ft;
        FT_Face face;
        Rectangle* rect;
        unsigned int fontSize;

        bool addChar(char c);
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
