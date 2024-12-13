#pragma once

#include "events/Forward.hpp"
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

        struct Options {
            BIRDY3D_WIDGET_OPTIONS_STRUCT
            Mode mode;
        };

        Mode mode;

        WindowSnapArea(Options);
        void on_update() override;
        virtual void draw() override;

    private:
        std::vector<Window*> m_windows;

        void on_click_raw(events::InputClickEvent const&);
        void on_resize_raw(events::WindowResizeEvent const&);
        void rearrange_windows();
    };

}
