#pragma once

#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    class FPSCounter : public Widget {
    public:
        Text* text;

        FPSCounter(Options options)
            : Widget(options) {
            text = add_text(0_px, "FPS: ", utils::Color::Name::FG, Placement::CENTER_LEFT);
        }

        void on_update() override {
            int fps = 1 / core::Application::delta_time;
            text->text("FPS: " + std::to_string(fps));
        }

        glm::vec2 minimal_size() override {
            glm::vec2 min_size = core::Application::theme().text_renderer().text_size("FPS: 000") + 2.0f;
            return glm::max(min_size, Widget::minimal_size());
        }
    };

}
