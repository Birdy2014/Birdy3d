#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class CheckBox : public Widget {
    public:
        bool checked = false;
        std::function<void(bool)> change_callback = nullptr;

        CheckBox(UIVector pos, Placement placement, std::string text);
        void draw() override;
        bool onClick(InputClickEvent*, bool) override;
        std::string text();
        void text(std::string text);

    private:
        Text* m_text_shape;
        Rectangle* m_check_shape;
    };

}
