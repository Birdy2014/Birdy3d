#pragma once

#include "ui/widgets/TextField.hpp"

namespace Birdy3d {

    class NumberInput : public TextField {
    public:
        NumberInput(UIVector position, UIVector size, Placement placement, float val);
        float value();
        void value(float value);

    private:
        float m_value;

        bool onScroll(InputScrollEvent* event, bool hover) override;
        bool onClick(InputClickEvent* event, bool hover) override;
        bool onKey(InputKeyEvent* event, bool hover) override;
        bool onChar(InputCharEvent* event, bool hover) override;
    };

}
