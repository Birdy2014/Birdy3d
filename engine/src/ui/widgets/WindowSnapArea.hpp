#pragma once

#include "events/WindowResizeEvent.hpp"
#include "ui/Widget.hpp"
#include <vector>

namespace Birdy3d {

    class Window;

    class WindowSnapArea : public Widget {
    public:
        WindowSnapArea(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), Placement placement = Placement::BOTTOM_LEFT);

    private:
        Rectangle* m_background_rect;
        std::vector<Window*> m_windows;

        void on_click_raw(InputClickEvent*);
        void on_resize_raw(WindowResizeEvent*);
        void recalculate_sizes();
    };

}
