#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class InputScrollEvent;

    class Textarea : public Widget {
    public:
        std::string text;
        float scrollpos;

        Textarea(UIVector pos, UIVector size, Placement placement);
        void draw() override;

    private:
        float tmpscroll;

        std::vector<std::string> getLines();
        void onScroll(InputScrollEvent* event);
    };

}
