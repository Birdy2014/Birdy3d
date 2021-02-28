#pragma once

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
        void renderText(std::string text, float x, float y, float fontSize, Color color, glm::mat4 move);
        UIVector textSize(std::string text, float fontSize);

    private:
        std::map<char, Character> chars;
        FT_Library ft;
        FT_Face face;
        Rectangle* rect;
        unsigned int fontSize;

        bool addChar(char c);
    };

    class Text {
    public:
        UIVector pos;
        float fontSize;
        std::string text;
        Color color;
        Placement placement;
        TextRenderer* renderer;

        Text(UIVector pos, float fontSize, std::string text, Color color, Placement placement, TextRenderer* renderer)
            : pos(pos)
            , fontSize(fontSize)
            , text(text)
            , color(color)
            , placement(placement)
            , renderer(renderer) { }
        void calcPos(glm::vec2 parentSize);
        void render(glm::mat4 move);

    private:
        glm::vec2 relativePos;
    };

}
