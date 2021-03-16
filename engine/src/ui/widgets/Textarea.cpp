#include "ui/widgets/Textarea.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "events/InputClickEvent.hpp"
#include "events/InputScrollEvent.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    Textarea::Textarea(UIVector pos, UIVector size, Placement placement)
        : Widget(pos, size, placement) {
        scrollpos = 0;
        tmpscroll = 0;
        Theme* theme = Application::defaultTheme;
        addFilledRectangle(0_px, 100_p, theme->color_bg);
        Application::eventBus->subscribe(this, &Textarea::onClick);
        Application::eventBus->subscribe(this, &Textarea::onScroll);
    }

    void Textarea::append(const std::string& text) {
        this->text += text;
        lines = getLines();
    }

    void Textarea::arrange(glm::mat4 move, glm::vec2 size) {
        Widget::arrange(move, size);
        lines = getLines();
    }

    void Textarea::draw() {
        Widget::draw();
        if (selecting)
            updateCursorEnd();
        int linec = actualSize.y / theme->fontSize;
        size_t line;
        for (int l = 0; l < linec + 1; l++) {
            // smooth scrolling
            float scrolldelta = (scrollpos - tmpscroll) * Application::deltaTime;
            tmpscroll += scrolldelta;
            if (scrolldelta < 0.0002 && scrolldelta > -0.0002)
                tmpscroll = scrollpos;

            // draw lines
            line = l + floor(tmpscroll);
            int y = actualSize.y - (l + 1) * theme->fontSize + (tmpscroll - floor(tmpscroll)) * theme->fontSize;
            if (line >= 0 && line < lines.size()) {
                float topOffset = actualSize.y - y - theme->fontSize;
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
                Application::getTextRenderer()->renderText(lines[line], 0, y, theme->fontSize, theme->color_fg, normalizedMove(), topOffset < 0 ? -topOffset : 0, y < 0 ? -y : 0, line == textCursorY ? textCursorX : -1, highlightstart, highlightend, "#0000a050");
            }
        }
    }

    std::vector<std::string> Textarea::getLines() {
        TextRenderer* renderer = Application::getTextRenderer();
        std::vector<std::string> lines;
        std::string line;
        size_t pos = 0, eol = 0, nextspace = 0, prevspace = 0, length = 0;
        while (pos != std::string::npos && pos < text.length()) {
            eol = text.find_first_of('\n', pos);
            if (eol == std::string::npos)
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
            if (textCursor >= pos && textCursor < eol) {
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
        return lines;
    }

    void Textarea::onClick(InputClickEvent* event) {
        if (!hover && event->action != GLFW_RELEASE)
            return;
        if (event->action != GLFW_PRESS && event->action != GLFW_RELEASE)
            return;

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
            selectionEnd = charPos.z;
            selectionEndX = charPos.x;
            selectionEndY = charPos.y;
        }
    }

    void Textarea::onScroll(InputScrollEvent* event) {
        if (!hover)
            return;
        scrollpos -= event->yoffset;
        if (scrollpos < 0)
            scrollpos = 0;
    }

    void Textarea::updateCursorEnd() {
        if (!hover)
            return;

        glm::ivec3 charPos = cursorCharPos();

        selectionEnd = charPos.z;
        selectionEndX = charPos.x;
        selectionEndY = charPos.y;
    }

    glm::ivec3 Textarea::cursorCharPos() {
        glm::vec2 absPos = move * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec2 localPos = Input::cursorPos() - absPos;

        int y = tmpscroll + (actualSize.y - localPos.y) / theme->fontSize;
        if (y >= lines.size()) y = lines.size() - 1;
        int x = -1;
        float width = 0;
        std::string& line = lines[y];
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
