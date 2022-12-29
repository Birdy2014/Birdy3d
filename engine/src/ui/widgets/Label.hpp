#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    class Label : public Widget {
    public:
        TextDescription text;

        struct Options {
            BIRDY3D_WIDGET_OPTIONS_STRUCT
            std::string text;
        };

        Label(Options options)
            : Widget(options)
            , text(options.text){};

        glm::ivec2 minimal_size() override
        {
            return text.text_size();
        }

        void draw() override
        {
            paint_text(0_px, Placement::CENTER_LEFT, text);
        }
    };

}
