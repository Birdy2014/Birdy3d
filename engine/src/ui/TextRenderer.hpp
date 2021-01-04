#ifndef BIRDY3D_TEXTRENDERER_HPP
#define BIRDY3D_TEXTRENDERER_HPP

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "render/Shader.hpp"
#include "ui/Rectangle.hpp"

struct Character {
    unsigned int textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};

class TextRenderer {
public:
    TextRenderer(std::string path, unsigned int fontSize);
    ~TextRenderer();
    void renderText(std::string text, float x, float y, float fontSize, glm::vec4 color);
    void renderText(std::string text, float x, float y, float fontSize, glm::vec4 color, glm::mat4 move);

private:
    std::map<char, Character> chars;
    FT_Library ft;
    FT_Face face;
    Rectangle *rect;
    unsigned int fontSize;

    bool addChar(char c);
};

#endif
