#pragma once

#include "ui/TextRenderer.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    class Label : public Widget {
    public:
        struct Options {
            BIRDY3D_WIDGET_OPTIONS_STRUCT
            std::string text;
        };

        Label(Options options)
            : Widget(options)
        {
            m_text = add_text(0_px, options.text, utils::Color::Name::FG, Placement::CENTER_LEFT);
        };

        glm::ivec2 minimal_size() override
        {
            return m_text->size().to_pixels();
        }

    protected:
        Text* m_text;
    };

}
