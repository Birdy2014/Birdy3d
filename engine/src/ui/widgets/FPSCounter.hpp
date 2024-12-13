#pragma once

#include "core/Application.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    class FPSCounter : public Widget {
    public:
        FPSCounter(Options options)
            : Widget(options)
            , m_text("FPS: ")
        { }

        void on_update() override
        {
            int fps = 1 / core::Application::delta_time;
            m_text.text("FPS: " + std::to_string(fps));
        }

        glm::ivec2 minimal_size() override
        {
            return m_text.text_size();
        }

        void draw() override
        {
            paint_text(0_px, Placement::CENTER_LEFT, m_text);
        }

    private:
        TextDescription m_text;
    };

}
