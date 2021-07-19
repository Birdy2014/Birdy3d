#pragma once

#include "events/InputEvents.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d {

    class Window : public Widget {
    public:
        const int BORDER_SIZE = 4;
        std::function<void()> callback_close;

        Window(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), Theme* theme = Application::defaultTheme, std::string name = "");
        void toForeground();

        std::string title() { return m_title->text; }
        void title(std::string title) { m_title->text = title; }

    protected:
        void on_update() override;
        void on_click(InputClickEvent* event) override;
        void on_mouse_leave() override;

    private:
        Rectangle* closeButton;
        Text* m_title;
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
