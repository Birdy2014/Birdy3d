#include "ui/widgets/Textarea.hpp"

#include "core/Application.hpp"
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
        Application::eventBus->subscribe(this, &Textarea::onScroll);
    }

    void Textarea::draw() {
        Widget::draw();
        int linec = actualSize.y / theme->fontSize;
        std::vector<std::string> lines = getLines();
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
                Application::getTextRenderer()->renderText(lines[line], 0, y, theme->fontSize, theme->color_fg, normalizedMove(), topOffset < 0 ? -topOffset : 0, y < 0 ? -y : 0);
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
            pos = eol + 1;
            lines.push_back(line);
        }
        return lines;
    }

    void Textarea::onScroll(InputScrollEvent* event) {
        if (hover)
            scrollpos += event->yoffset;
        if (scrollpos < 0)
            scrollpos = 0;
    }
}
