#pragma once

#include "events/InputEvents.hpp"
#include "ui/Rectangle.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d {

    class Window : public Widget {
    public:
        const int BORDER_SIZE = 4;
        const int BAR_HEIGHT = 14;

        Window(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), Theme* theme = Application::defaultTheme, std::string name = "");
        void arrange(glm::vec2 pos, glm::vec2 size) override;
        void toForeground();
        void draw() override;
        glm::vec2 minimalSize() override;
        void set_child(Widget*);
        void set_canvas(Canvas*) override;
        bool update_hover(bool hover) override;
        void late_update() override;

    protected:
        void on_update() override;
        void on_click(InputClickEvent* event) override;
        void on_mouse_leave() override;

    private:
        Widget* m_child = nullptr;
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
