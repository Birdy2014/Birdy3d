#include "ui/TextRenderer.hpp"

#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

TextRenderer::~TextRenderer() {
    FT_Done_Face(this->face);
    FT_Done_FreeType(this->ft);
}

bool TextRenderer::init(std::string path, unsigned int fontSize) {
    this->fontSize = fontSize;
    if (FT_Init_FreeType(&this->ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return false;
    }
    if (FT_New_Face(this->ft, path.c_str(), 0, &this->face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    this->rect = new Rectangle(glm::ivec2(0), glm::ivec2(0), glm::vec4(1), 0);
}

bool TextRenderer::addChar(char c) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if (FT_Load_Char(this->face, c, FT_LOAD_RENDER)) {
        std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
        return false;
    }
    // generate texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->face->glyph->bitmap.width, this->face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // store character
    Character character = {
        texture,
        glm::ivec2(this->face->glyph->bitmap.width, this->face->glyph->bitmap.rows),
        glm::ivec2(this->face->glyph->bitmap_left, this->face->glyph->bitmap_top),
        this->face->glyph->advance.x
    };
    this->chars.insert(std::pair<char, Character>(c, character));
}

void TextRenderer::renderText(std::string text, float x, float y, float fontSize, glm::vec4 color) {
    glm::vec2 viewportSize = Application::getViewportSize();
    glm::mat4 m = glm::ortho(0.0f, viewportSize.x, 0.0f, viewportSize.y);
    this->renderText(text, x, y, fontSize, color, m);
}

void TextRenderer::renderText(std::string text, float x, float y, float fontSize, glm::vec4 color, glm::mat4 move) {
    float scale = (fontSize / this->fontSize);
    this->rect->setColor(color);
    this->rect->setMove(move);
    for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
        if (chars.count(*c) == 0) {
            this->addChar(*c);
        }
        Character ch = this->chars[*c];
        float xpos = x + ch.bearing.x * scale;
        float ypos = y;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        this->rect->setPos(glm::ivec2(xpos, ypos));
        this->rect->resize(glm::ivec2(w, h));
        this->rect->setTexture(ch.textureID);
        this->rect->draw();
        x += (ch.advance >> 6) * scale;
    }
}
