#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    class CheckBox : public Widget {
    public:
        bool checked = false;
        std::function<void(bool)> callback_change;

        CheckBox(UIVector pos, Placement placement, std::string text);
        void draw() override;
        std::string text();
        void text(std::string text);

    private:
        Text* m_text_shape;
        Rectangle* m_check_shape;

        void on_click(const events::InputClickEvent&) override;
    };

}
