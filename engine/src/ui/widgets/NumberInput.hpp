#pragma once

#include "ui/widgets/TextField.hpp"
#include <limits>

namespace Birdy3d {

    class NumberInput : public TextField {
    public:
        float max_value = std::numeric_limits<int>::max();
        float min_value = std::numeric_limits<int>::min();

        NumberInput(UIVector position, UIVector size, Placement placement, float val);
        float value();
        void value(float value);

    private:
        float m_value;
        bool m_dragging = false;

        void on_update() override;
        void on_scroll(InputScrollEvent* event) override;
        void on_click(InputClickEvent* event) override;
        void on_key(InputKeyEvent* event) override;
        void on_char(InputCharEvent* event) override;
        void on_focus_lost() override;
    };

}
