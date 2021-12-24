#pragma once

#include "ui/TextRenderer.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    class Label : public Widget {
    public:
        Label(Options options, std::string text)
            : Widget(options) {
            m_text = add_text(UIVector(0_px), text, utils::Color::Name::FG, Placement::CENTER_LEFT);
        };

        glm::vec2 minimal_size() override {
            return m_text->size();
        }

    protected:
        Text* m_text;
    };

}
