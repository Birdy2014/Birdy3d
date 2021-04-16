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
        this->rect = new Rectangle(UIVector(0), UIVector(0), Color::WHITE, Rectangle::Type::TEXT);
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

    void TextRenderer::renderText(std::string text, float x, float y, float fontSize, Color color) {
        glm::vec2 viewportSize = Application::getViewportSize();
        glm::mat4 m = glm::ortho(0.0f, viewportSize.x, 0.0f, viewportSize.y);
        this->renderText(text, x, y, fontSize, color, m);
    }

    void TextRenderer::renderText(std::string text, float x, float y, float fontSize, Color color, glm::mat4 move, float cutTop, float cutBottom, int cursorpos, int hlstart, int hlend, Color hlcolor) {
        if (cutTop < 0)
            cutTop = 0;

        bool highlighting = false;
        float scale = (fontSize / this->fontSize);
        float cursorHeight = (fontSize * 1.1);
        char c;
        for (size_t i = 0; i <= text.length(); i++) {
            if (i < text.length()) c = text[i];
            else c = ' ';

            if (i == hlstart)
                highlighting = true;
            if (hlend >= 0 && i == hlend + 1)
                highlighting = false;

            if (chars.count(c) == 0) {
                this->addChar(c);
            }
            Character ch = this->chars[c];
            float bottomToOrigin = (ch.size.y - ch.bearing.y) / 1.5; // For some reason, the division by 1.5 is necessary.
            float currentCutBottom = std::max(cutBottom, -bottomToOrigin);
            float xpos = x + ch.bearing.x * scale;
            float ypos = y;
            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            this->rect->position(UIVector(xpos, ypos + currentCutBottom));
            this->rect->texture(ch.textureID);
            this->rect->color(color);

            // Cut text
            float topSpace = fontSize + bottomToOrigin - h;
            float newCutTop = cutTop - topSpace;
            if (newCutTop < 0) newCutTop = 0;
            float cutTopTex = newCutTop / h;
            float cutBottomTex = (bottomToOrigin ? (currentCutBottom + bottomToOrigin) : currentCutBottom) / h;

            this->rect->size(UIVector(w, h - newCutTop - (bottomToOrigin ? currentCutBottom + bottomToOrigin : currentCutBottom)));
            this->rect->texCoords(glm::vec2(0, cutTopTex), glm::vec2(1, 1 - cutBottomTex));

            this->rect->draw(move);

            currentCutBottom = std::max(cutBottom, -(cursorHeight / 6));

            if (i == cursorpos) {
                this->rect->type = Rectangle::FILLED;
                this->rect->position(UIVector(xpos - 2, ypos + currentCutBottom));
                this->rect->size(UIVector(2, std::min(cursorHeight - cutTop - currentCutBottom - (cursorHeight / 6), cursorHeight)));
                this->rect->draw(move);

                this->rect->type = Rectangle::TEXT;
            }

            if (highlighting && i < text.length()) {
                this->rect->type = Rectangle::FILLED;
                this->rect->color(hlcolor);
                this->rect->position(UIVector(xpos - 2, ypos + currentCutBottom));
                this->rect->size(UIVector((ch.advance >> 6) * scale, std::min(cursorHeight - cutTop - currentCutBottom - (cursorHeight / 6), cursorHeight)));
                this->rect->draw(move);

                this->rect->color(color);
                this->rect->type = Rectangle::TEXT;
            }

            x += (ch.advance >> 6) * scale;
        }
    }

    UIVector TextRenderer::textSize(std::string text, float fontSize) {
        float scale = (fontSize / this->fontSize);
        UIVector size(0_px);
        for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
            if (chars.count(*c) == 0)
                this->addChar(*c);
            Character ch = this->chars[*c];
            float h = ch.size.y * scale;
            size.y = std::max((float)size.y, h);
            size.x += (ch.advance >> 6) * scale;
        }
        return size;
    }

    float TextRenderer::charWidth(char c, float fontSize) {
        if (chars.count(c) == 0)
            this->addChar(c);
        Character ch = this->chars[c];
        return (ch.advance >> 6) * (fontSize / this->fontSize);
    }

    void Text::calcPos(glm::vec2 parentSize) {
        UIVector textSize = renderer->textSize(text, fontSize);
        relativePos = Utils::getRelativePosition(pos, textSize, parentSize, placement);
    }

    void Text::render(glm::mat4 move) {
        renderer->renderText(text, relativePos.x, relativePos.y, fontSize, color, move);
    }

}
