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
        m_tmpscroll = 0;
        this->readonly = readonly;
        addFilledRectangle(0_px, 100_p, theme->color_bg);
    }

    void Textarea::append(const std::string& text) {
        m_text += theme->text_renderer()->converter.from_bytes(text);
        updateLines();
    }

    void Textarea::arrange(glm::vec2 pos, glm::vec2 size) {
        Widget::arrange(pos, size);
        updateLines();
    }

    void Textarea::draw() {
        Widget::draw();
        if (m_selecting)
            updateCursorEnd();
        int linec = actualSize.y / theme->lineHeight;
        size_t line;
        for (int l = 0; l < linec + 1; l++) {
            // smooth scrolling
            float scrolldelta = (scrollpos - m_tmpscroll) * Application::deltaTime;
            m_tmpscroll += scrolldelta;
            if (scrolldelta < 0.0002 && scrolldelta > -0.0002)
                m_tmpscroll = scrollpos;

            // draw lines
            line = l + floor(m_tmpscroll);
            int y = actualSize.y - (l + 1) * theme->lineHeight + (m_tmpscroll - floor(m_tmpscroll)) * theme->lineHeight;
            if (line >= 0 && line < m_lines.size()) {
                int highlightstart = -1;
                int highlightend = -1;
                if (m_selectionStart < m_selectionEnd) {
                    if (line == m_selectionStartY)
                        highlightstart = m_selectionStartX;
                    if (line > m_selectionStartY && line <= m_selectionEndY)
                        highlightstart = 0;
                    if (line == m_selectionEndY)
                        highlightend = m_selectionEndX;
                } else if (m_selectionStart > m_selectionEnd) {
                    if (line == m_selectionStartY)
                        highlightend = m_selectionStartX;
                    if (line <= m_selectionStartY && line > m_selectionEndY)
                        highlightstart = 0;
                    if (line == m_selectionEndY)
                        highlightstart = m_selectionEndX;
                }
                theme->text_renderer()->renderText(m_lines[line], 0, y, theme->fontSize, theme->color_fg, normalizedMove(), line == m_textCursorY ? m_textCursorX : -1, highlightstart, highlightend - 1, "#0000a050");
            }
        }
    }

    void Textarea::updateLines() {
        m_lines.clear();
        std::u32string line;
        size_t pos = 0, eol = 0, nextspace = 0, prevspace = 0, length = 0;
        while (pos != std::u32string::npos && pos < m_text.length()) {
            eol = m_text.find_first_of('\n', pos);
            if (eol == std::u32string::npos)
                eol = m_text.length();

            line = m_text.substr(pos, eol - pos);
            // Line is too long
            length = theme->text_renderer()->textSize(line, theme->fontSize).x;
            if (length > actualSize.x) {
                nextspace = pos;
                while (nextspace < eol) {
                    prevspace = nextspace;
                    nextspace = m_text.find_first_of(' ', prevspace + 1);
                    line = m_text.substr(pos, nextspace - pos);

                    // reached the space too far right
                    length = theme->text_renderer()->textSize(line, theme->fontSize).x;
                    if (length > actualSize.x) {
                        // the line can't be broken using a space
                        if (prevspace == pos)
                            prevspace = eol;
                        line = m_text.substr(pos, prevspace - pos);
                        break;
                    }
                }
                eol = prevspace;
            }
            if (m_textCursor >= pos && m_textCursor <= eol) {
                m_textCursorX = m_textCursor - pos;
                m_textCursorY = m_lines.size();
            }
            if (m_selectionStart >= pos && m_selectionStart < eol) {
                m_selectionStartX = m_selectionStart - pos;
                m_selectionStartY = m_lines.size();
            }
            if (m_selectionEnd >= pos && m_selectionEnd < eol) {
                m_selectionEndX = m_selectionEnd - pos;
                m_selectionEndY = m_lines.size();
            }
            pos = eol + 1;
            m_lines.push_back(line);
        }
    }

    bool Textarea::onClick(InputClickEvent* event, bool hover) {
        if (readonly || !hover && event->action != GLFW_RELEASE)
            return true;

        glm::ivec3 charPos = cursorCharPos();

        if (event->action == GLFW_PRESS) {
            m_selecting = true;
            m_selectionStart = charPos.z;
            m_selectionStartX = charPos.x;
            m_selectionStartY = charPos.y;
            m_textCursor = -1;
            m_textCursorX = -1;
            m_textCursorY = -1;
        } else if (event->action == GLFW_RELEASE && charPos.z == m_selectionStart) {
            m_selecting = false;
            m_textCursor = m_selectionStart;
            m_textCursorX = m_selectionStartX;
            m_textCursorY = m_selectionStartY;
            m_selectionStart = -1;
            m_selectionStartX = -1;
            m_selectionStartY = -1;
            m_selectionEnd = -1;
            m_selectionEndX = -1;
            m_selectionEndY = -1;
        } else if (event->action == GLFW_RELEASE && charPos.z != m_selectionStart) {
            m_selecting = false;
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
        if (readonly || !hover || m_selectionStart < 0 && m_textCursor < 0)
            return true;
        clearSelection();
        m_text.insert(m_textCursor, (char32_t*)&event->codepoint);
        m_textCursor++;
        updateLines();
        return true;
    }

    // TODO: key repeat
    bool Textarea::onKey(InputKeyEvent* event, bool hover) {
        if (readonly || !hover || event->action != GLFW_PRESS)
            return true;
        if (m_selectionStart >= 0 && m_selectionEnd >= 0) {
            if (event->key == GLFW_KEY_DELETE || event->key == GLFW_KEY_BACKSPACE) {
                clearSelection();
            }
        } else if (m_textCursor > 0) {
            switch (event->key) {
            case GLFW_KEY_DELETE:
                if (m_textCursor >= m_text.length())
                    break;
                m_text.erase(m_text.begin() + m_textCursor, m_text.begin() + m_textCursor + 1);
                break;
            case GLFW_KEY_BACKSPACE:
                if (m_textCursor <= 0)
                    break;
                m_text.erase(m_text.begin() + m_textCursor - 1, m_text.begin() + m_textCursor);
                m_textCursor--;
                break;
            case GLFW_KEY_LEFT:
                if (m_textCursor <= 0)
                    break;
                m_textCursor--;
                break;
            case GLFW_KEY_RIGHT:
                if (m_textCursor >= m_text.length())
                    break;
                m_textCursor++;
                break;
            case GLFW_KEY_UP:
                if (m_textCursorY <= 0)
                    break;
                m_textCursor -= (m_lines[m_textCursorY - 1].size() + 1) > m_textCursorX ? (m_lines[m_textCursorY - 1].size() + 1) : m_textCursorX + 1;
                break;
            case GLFW_KEY_DOWN: {
                if (m_textCursorY >= m_lines.size())
                    break;
                int lineLength = m_lines[m_textCursorY].size();
                if (m_lines[m_textCursorY + 1].size() + 1 > m_textCursorX)
                    m_textCursor += lineLength + 1;
                else
                    m_textCursor += lineLength - m_textCursorX + 1 + m_lines[m_textCursorY + 1].size();
                break;
            }
            case GLFW_KEY_ENTER: {
                m_text.insert(m_textCursor, U"\n");
                m_textCursor++;
                break;
            }
            }
        }
        return true;
    }

    void Textarea::onMouseEnter() {
        if (!readonly)
            Input::setCursor(Input::CURSOR_TEXT);
    }

    void Textarea::onMouseLeave() {
        if (!readonly)
            Input::setCursor(Input::CURSOR_DEFAULT);
    }

    void Textarea::clearSelection() {
        if (m_selectionStart != -1 && m_selectionEnd != -1) {
            m_text.erase(m_text.begin() + m_selectionStart, m_text.begin() + m_selectionEnd);
            m_textCursor = m_selectionStart;
            m_selectionStart = -1;
            m_selectionEnd = -1;
            m_selecting = false;
        }
    }

    void Textarea::updateCursorEnd() {
        glm::ivec3 charPos = cursorCharPos();

        m_selectionEnd = charPos.z;
        m_selectionEndX = charPos.x;
        m_selectionEndY = charPos.y;
    }

    glm::ivec3 Textarea::cursorCharPos() {
        glm::vec2 localPos = Input::cursorPos() - actualPos;

        int y = m_tmpscroll + (actualSize.y - localPos.y) / theme->lineHeight;
        if (y >= m_lines.size())
            y = m_lines.size() - 1;
        int x = -1;
        float width = 0;
        std::u32string line = m_lines[y];
        for (int i = 0; i < line.length(); i++) {
            float charWidth = theme->text_renderer()->charWidth(line[i], theme->fontSize);
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
            pos += m_lines[i].length() + 1;

        return glm::ivec3(x, y, pos);
    }
}
