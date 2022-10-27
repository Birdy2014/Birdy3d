#pragma once

#include "ui/widgets/TextField.hpp"
#include <limits>

namespace Birdy3d::ui {

    class NumberInput : public TextField {
    public:
        struct Options {
            BIRDY3D_WIDGET_OPTIONS_STRUCT
            float value{0};
        };

        float max_value = std::numeric_limits<float>::infinity();
        float min_value = -std::numeric_limits<float>::infinity();

        NumberInput(Options);
        float value();
        void value(float value);

    private:
        float m_value;
        bool m_dragging = false;

        void on_update() override;
        void on_scroll(ScrollEvent&) override;
        void on_click(ClickEvent&) override;
        void on_key(KeyEvent&) override;
        void on_char(CharEvent&) override;
        void on_focus_lost(FocusLostEvent&) override;

        void set_text_value();
    };

}
