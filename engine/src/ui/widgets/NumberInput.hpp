#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class NumberInput : public Widget {
    public:
        std::function<void(float)> change_callback = nullptr;

        NumberInput(UIVector position, Placement placement, float val);
        float value();
        void value(float value);

    private:
        float m_value;
        Text* m_value_text;

        bool onScroll(InputScrollEvent* event, bool hover) override;
        bool onClick(InputClickEvent* event, bool hover) override;
    };

}
