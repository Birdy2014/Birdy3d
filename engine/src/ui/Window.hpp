#pragma once

#include "events/InputEvents.hpp"
#include "ui/Widget.hpp"
#include "ui/Rectangle.hpp"

namespace Birdy3d {

    class Window : public Widget {
    public:
        const int BORDER_SIZE = 4;
        const int BAR_HEIGHT = 14;
        Widget* child;

        Window(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), Theme* theme = Application::defaultTheme, std::string name = "");
        void arrange(glm::vec2 pos, glm::vec2 size) override;
        void draw() override;
        glm::vec2 minimalSize() override;

    protected:
        bool update(bool hover) override;
        bool onScroll(InputScrollEvent* event, bool hover) override;
        bool onClick(InputClickEvent* event, bool hover) override;
        bool onKey(InputKeyEvent* event, bool hover) override;
        bool onChar(InputCharEvent* event, bool hover) override;

    private:
        Rectangle* closeButton;
        bool hoverDrag = false;
        bool hoverResizeXL = false;
        bool hoverResizeXR = false;
        bool hoverResizeY = false;
        bool dragging = false;
        bool resizeXL = false;
        bool resizeXR = false;
        bool resizeY = false;
    };

}
