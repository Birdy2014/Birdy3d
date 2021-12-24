#pragma once

#include "ui/widgets/TextField.hpp"
#include <limits>

namespace Birdy3d::ui {

    class NumberInput : public TextField {
    public:
        float max_value = std::numeric_limits<float>::infinity();
        float min_value = -std::numeric_limits<float>::infinity();

        NumberInput(Options, float val = 0);
        float value();
        void value(float value);

    private:
        float m_value;
        bool m_dragging = false;

        void on_update() override;
        void on_scroll(const events::InputScrollEvent& event) override;
        void on_click(const events::InputClickEvent& event) override;
        void on_key(const events::InputKeyEvent& event) override;
        void on_char(const events::InputCharEvent& event) override;
        void on_focus_lost() override;
    };

}
