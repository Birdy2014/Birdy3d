#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class Layout : public Widget {
    public:
        Layout(UIVector pos = UIVector(0), UIVector size = UIVector(0), Placement placement = Placement::BOTTOM_LEFT);
        void addChild(Widget* w);
        void draw() override;
        virtual void arrange(glm::mat4 move, glm::vec2 size) override;
        bool updateEvents() override;

    protected:
        std::vector<Widget*> children;
    };

}
