#include "ui/widgets/Textarea.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "events/InputEvents.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    Textarea::Textarea(UIVector pos, UIVector size, Placement placement, bool readonly)
        : Widget(pos, size, placement) {
        scrollpos = 0;
        tmpscroll = 0;
        this->readonly = readonly;
        Theme* theme = Application::defaultTheme;
        renderer = Application::getTextRenderer();
        addFilledRectangle(0_px, 100_p, theme->color_bg);
    }

    void Textarea::append(const std::string& text) {
        this->text += renderer->converter.from_bytes(text);
        updateLines();
    }

    void Textarea::arrange(glm::vec2 pos, glm::vec2 size) {
        Widget::arrange(pos, size);
        updateLines();
    }

    void Textarea::draw() {
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF); // If glStencilMask(0xFF) is not called before glClear, stuff breaks
        glClear(GL_STENCIL_BUFFER_BIT);
        Widget::draw();
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);
        if (selecting)
            updateCursorEnd();
        int linec = actualSize.y / theme->lineHeight;
        size_t line;
        for (int l = 0; l < linec + 1; l++) {
            // smooth scrolling
            float scrolldelta = (scrollpos - tmpscroll) * Application::deltaTime;
            tmpscroll += scrolldelta;
            if (scrolldelta < 0.0002 && scrolldelta > -0.0002)
                tmpscroll = scrollpos;

            // draw lines
            line = l + floor(tmpscroll);
            int y = actualSize.y - (l + 1) * theme->lineHeight + (tmpscroll - floor(tmpscroll)) * theme->lineHeight;
            if (line >= 0 && line < lines.size()) {
                int highlightstart = -1;
                int highlightend = -1;
                if (selectionStart < selectionEnd) {
                    if (line == selectionStartY)
                        highlightstart = selectionStartX;
                    if (line > selectionStartY && line <= selectionEndY)
                        highlightstart = 0;
                    if (line == selectionEndY)
                        highlightend = selectionEndX;
                } else if (selectionStart > selectionEnd) {
                    if (line == selectionStartY)
                        highlightend = selectionStartX;
                    if (line <= selectionStartY && line > selectionEndY)
                        highlightstart = 0;
                    if (line == selectionEndY)
                        highlightstart = selectionEndX;
                }
                renderer->renderText(lines[line], 0, y, theme->fontSize, theme->color_fg, normalizedMove(), line == textCursorY ? textCursorX : -1, highlightstart, highlightend - 1, "#0000a050");
            }
        }
        glDisable(GL_STENCIL_TEST);
    }

    void Textarea::updateLines() {
        lines.clear();
        std::u32string line;
        size_t pos = 0, eol = 0, nextspace = 0, prevspace = 0, length = 0;
        while (pos != std::u32string::npos && pos < text.length()) {
            eol = text.find_first_of('\n', pos);
            if (eol == std::u32string::npos)
                eol = text.length();

            line = text.substr(pos, eol - pos);
            // Line is too long
            length = renderer->textSize(line, theme->fontSize).x;
            if (length > actualSize.x) {
                nextspace = pos;
                while (nextspace < eol) {
                    prevspace = nextspace;
                    nextspace = text.find_first_of(' ', prevspace + 1);
                    line = text.substr(pos, nextspace - pos);

                    // reached the space too far right
                    length = renderer->textSize(line, theme->fontSize).x;
                    if (length > actualSize.x) {
                        // the line can't be broken using a space
                        if (prevspace == pos)
                            prevspace = eol;
                        line = text.substr(pos, prevspace - pos);
                        break;
                    }
                }
                eol = prevspace;
            }
            if (textCursor >= pos && textCursor <= eol) {
                textCursorX = textCursor - pos;
                textCursorY = lines.size();
            }
            if (selectionStart >= pos && selectionStart < eol) {
                selectionStartX = selectionStart - pos;
                selectionStartY = lines.size();
            }
            if (selectionEnd >= pos && selectionEnd < eol) {
                selectionEndX = selectionEnd - pos;
                selectionEndY = lines.size();
            }
            pos = eol + 1;
            lines.push_back(line);
        }
    }

    bool Textarea::onClick(InputClickEvent* event, bool hover) {
        if (readonly || !hover && event->action != GLFW_RELEASE)
            return true;

        glm::ivec3 charPos = cursorCharPos();

        if (event->action == GLFW_PRESS) {
            selecting = true;
            selectionStart = charPos.z;
            selectionStartX = charPos.x;
            selectionStartY = charPos.y;
            textCursor = -1;
            textCursorX = -1;
            textCursorY = -1;
        } else if (event->action == GLFW_RELEASE && charPos.z == selectionStart) {
            selecting = false;
            textCursor = selectionStart;
            textCursorX = selectionStartX;
            textCursorY = selectionStartY;
            selectionStart = -1;
            selectionStartX = -1;
            selectionStartY = -1;
            selectionEnd = -1;
            selectionEndX = -1;
            selectionEndY = -1;
        } else if (event->action == GLFW_RELEASE && charPos.z != selectionStart) {
            selecting = false;
        }
        return true;
    }

    bool Textarea::onScroll(InputScrollEvent* event, bool hover) {
        if (!hover)
            return true;
        scrollpos -= event->yoffset;
        if (scrollpos < 0)
            scrollpos = 0;
        return true;
    }

    bool Textarea::onChar(InputCharEvent* event, bool hover) {
        if (readonly || !hover || selectionStart < 0 && textCursor < 0)
            return true;
        clearSelection();
        text.insert(textCursor, (char32_t*)&event->codepoint);
        textCursor++;
        updateLines();
        return true;
    }

    // TODO: key repeat
    bool Textarea::onKey(InputKeyEvent* event, bool hover) {
        if (readonly || !hover || event->action != GLFW_PRESS)
            return true;
        if (selectionStart >= 0 && selectionEnd >= 0) {
            if (event->key == GLFW_KEY_DELETE || event->key == GLFW_KEY_BACKSPACE) {
                clearSelection();
            }
        } else if (textCursor > 0) {
            switch (event->key) {
            case GLFW_KEY_DELETE:
                if (textCursor >= text.length())
                    break;
                text.erase(text.begin() + textCursor, text.begin() + textCursor + 1);
                break;
            case GLFW_KEY_BACKSPACE:
                if (textCursor <= 0)
                    break;
                text.erase(text.begin() + textCursor - 1, text.begin() + textCursor);
                textCursor--;
                break;
            case GLFW_KEY_LEFT:
                if (textCursor <= 0)
                    break;
                textCursor--;
                break;
            case GLFW_KEY_RIGHT:
                if (textCursor >= text.length())
                    break;
                textCursor++;
                break;
            case GLFW_KEY_UP:
                if (textCursorY <= 0)
                    break;
                textCursor -= (lines[textCursorY - 1].size() + 1) > textCursorX ? (lines[textCursorY - 1].size() + 1) : textCursorX + 1;
                break;
            case GLFW_KEY_DOWN: {
                if (textCursorY >= lines.size())
                    break;
                int lineLength = lines[textCursorY].size();
                if (lines[textCursorY + 1].size() + 1 > textCursorX)
                    textCursor += lineLength + 1;
                else
                    textCursor += lineLength - textCursorX + 1 + lines[textCursorY + 1].size();
                break;
            }
            case GLFW_KEY_ENTER: {
                text.insert(textCursor, U"\n");
                textCursor++;
                break;
            }
            }
        }
        return true;
    }

    void Textarea::clearSelection() {
        if (selectionStart != -1 && selectionEnd != -1) {
            text.erase(text.begin() + selectionStart, text.begin() + selectionEnd);
            textCursor = selectionStart;
            selectionStart = -1;
            selectionEnd = -1;
            selecting = false;
        }
    }

    void Textarea::updateCursorEnd() {
        glm::ivec3 charPos = cursorCharPos();

        selectionEnd = charPos.z;
        selectionEndX = charPos.x;
        selectionEndY = charPos.y;
    }

    glm::ivec3 Textarea::cursorCharPos() {
        glm::vec2 localPos = Input::cursorPos() - actualPos;

        int y = tmpscroll + (actualSize.y - localPos.y) / theme->lineHeight;
        if (y >= lines.size())
            y = lines.size() - 1;
        int x = -1;
        float width = 0;
        std::u32string line = lines[y];
        for (int i = 0; i < line.length(); i++) {
            float charWidth = Application::getTextRenderer()->charWidth(line[i], theme->fontSize);
            width += charWidth;
            // Correction for click between characters
            if (width > localPos.x + charWidth / 2) {
                x = i;
                break;
            }
            if (width > localPos.x) {
                x = i + 1;
                break;
            }
        }
        if (x == -1)
            x = line.length();

        int pos = x;
        for (int i = 0; i < y; i++)
            pos += lines[i].length() + 1;

        return glm::ivec3(x, y, pos);
    }
}
