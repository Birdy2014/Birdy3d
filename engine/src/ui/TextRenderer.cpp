#include "ui/TextRenderer.hpp"

#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "ui/Rectangle.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d {

    TextRenderer::~TextRenderer() {
        FT_Done_Face(this->face);
        FT_Done_FreeType(this->ft);
    }

    TextRenderer::TextRenderer(std::string path, unsigned int fontSize) {
        this->fontSize = fontSize;
        if (FT_Init_FreeType(&this->ft))
            Logger::error("freetype: Could not init FreeType Library");
        if (FT_New_Face(this->ft, path.c_str(), 0, &this->face))
            Logger::error("freetype: Failed to load font");
        FT_Set_Pixel_Sizes(face, 0, fontSize);
        this->rect = new Rectangle(glm::vec2(0), glm::vec2(0), glm::vec4(1), Shape::TEXT);
    }

    bool TextRenderer::addChar(char c) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        if (FT_Load_Char(this->face, c, FT_LOAD_RENDER)) {
            Logger::warn("freetype: Failed to load Glyph " + (unsigned)c);
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
        return true;
    }

    void TextRenderer::renderText(std::string text, float x, float y, float fontSize, glm::vec4 color) {
        glm::vec2 viewportSize = Application::getViewportSize();
        glm::mat4 m = glm::ortho(0.0f, viewportSize.x, 0.0f, viewportSize.y);
        this->renderText(text, x, y, fontSize, color, m);
    }

    void TextRenderer::renderText(std::string text, float x, float y, float fontSize, glm::vec4 color, glm::mat4 move) {
        float scale = (fontSize / this->fontSize);
        this->rect->color(color);
        for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
            if (chars.count(*c) == 0) {
                this->addChar(*c);
            }
            Character ch = this->chars[*c];
            float xpos = x + ch.bearing.x * scale;
            float ypos = y;
            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            this->rect->position(glm::vec2(xpos, ypos));
            this->rect->size(glm::vec2(w, h));
            this->rect->texture(ch.textureID);
            this->rect->draw(move);
            x += (ch.advance >> 6) * scale;
        }
    }

    glm::vec2 TextRenderer::textSize(std::string text, float fontSize) {
        float scale = (fontSize / this->fontSize);
        glm::vec2 size(0);
        for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
            if (chars.count(*c) == 0)
                this->addChar(*c);
            Character ch = this->chars[*c];
            float h = ch.size.y * scale;
            size.y = std::max(size.y, h);
            size.x += (ch.advance >> 6) * scale;
        }
        return size;
    }

    void Text::calcPos(glm::vec2 parentSize) {
        glm::vec2 textSize = renderer->textSize(text, fontSize);
        relativePos = Utils::getRelativePosition(pos, textSize, parentSize, placement, Unit::PIXELS);
    }

    void Text::render(glm::mat4 move) {
        renderer->renderText(text, relativePos.x, relativePos.y, fontSize, color, move);
    }

}
