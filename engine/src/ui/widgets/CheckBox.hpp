#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    class CheckBox : public Widget {
    public:
        bool checked = false;

        CheckBox(Options, std::string text);
        void draw() override;
        std::string text();
        void text(std::string text);

    private:
        Text* m_text_shape;
        Rectangle* m_check_shape;

        bool on_click(const events::InputClickEvent&) override;
    };

}
