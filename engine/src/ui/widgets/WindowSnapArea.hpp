#pragma once

#include "events/WindowResizeEvent.hpp"
#include "ui/Widget.hpp"
#include <vector>

namespace Birdy3d::ui {

    class Window;

    class WindowSnapArea : public Widget {
    public:
        enum class Mode {
            STACKING,
            HORIZONTAL,
            VERTICAL
        };

        Mode mode;

        WindowSnapArea(Options, Mode);
        void on_update() override;

    private:
        Rectangle* m_background_rect;
        std::vector<Window*> m_windows;

        void on_click_raw(const events::InputClickEvent&);
        void on_resize_raw(const events::WindowResizeEvent&);
        void rearrange_windows();
    };

}
