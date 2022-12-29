#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    class CheckBox : public Widget {
    public:
        struct Options {
            BIRDY3D_WIDGET_OPTIONS_STRUCT
            std::string text;
        };

        bool checked = false;
        std::function<void()> on_change;

        CheckBox(Options);
        void draw() override;
        std::string text();
        void text(std::string text);

    private:
        TextDescription m_text;

        void on_click(ClickEvent&) override;
    };

}
