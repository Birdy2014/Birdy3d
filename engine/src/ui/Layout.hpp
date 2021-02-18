#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class Layout : public Widget {
    public:
        Layout(glm::vec2 pos = glm::vec2(0.0f), glm::vec2 size = glm::vec2(0.0f), Placement placement = Placement::BOTTOM_LEFT, Unit unit = Unit::PIXELS);
        void addChild(Widget* w);
        void draw() override;
        virtual void arrange(glm::mat4 move, glm::vec2 size) override;
        bool updateEvents() override;

    protected:
        std::vector<Widget*> children;
    };

}
