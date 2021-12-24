#pragma once

#include "core/Input.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Triangle.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    class CollapsibleBox : public Widget {
    public:
        CollapsibleBox(Options options, std::string title)
            : Widget(options) {
            m_padding = glm::vec4(4, 4, 0, core::Application::theme().line_height());
            add_filled_rectangle(0_px, UIVector(100_p, 100_p - core::Application::theme().line_height()), utils::Color::Name::BG, Placement::BOTTOM_LEFT);
            add_filled_rectangle(0_px, UIVector(100_p, core::Application::theme().line_height()), utils::Color::Name::BG_TITLE_BAR, Placement::TOP_LEFT);
            m_arrow = add_filled_triangle(-4_px, 14_px, utils::Color::Name::FG, Placement::TOP_RIGHT);
            m_title = add_text(UIVector(10_px, 0_px), title, utils::Color::Name::FG, Placement::TOP_LEFT);
        }

        void title(std::string text) {
            m_title->text(text);
        }

        void toggle_collapsed() {
            m_children_visible = !m_children_visible;
            m_arrow->rotation(glm::radians(m_children_visible ? 60.0f : 0.0f));
        }

    private:
        Triangle* m_arrow;
        Text* m_title;

        void on_click(const events::InputClickEvent& event) override {
            if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS)
                return;
            auto local_cursor_pos = core::Input::cursor_pos() - m_actual_pos;
            if (local_cursor_pos.y < m_actual_size.y - core::Application::theme().line_height())
                return;
            toggle_collapsed();
        }
    };

}
