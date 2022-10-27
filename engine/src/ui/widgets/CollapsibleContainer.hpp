#pragma once

#include "core/Input.hpp"
#include "ui/Container.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Triangle.hpp"

namespace Birdy3d::ui {

    class CollapsibleContainer : public Container {
    public:
        struct Options {
            BIRDY3D_WIDGET_OPTIONS_STRUCT
            std::string title;
        };

        CollapsibleContainer(Options options)
            : Container(options)
        {
            m_padding = {
                .left = 4_px,
                .right = 4_px,
                .top = 1_em,
                .bottom = 0_px};
            add_filled_rectangle(0_px, Size(100_pc, 100_pc - 1_em), utils::Color::Name::BG, Placement::BOTTOM_LEFT);
            add_filled_rectangle(0_px, Size(100_pc, 1_em), utils::Color::Name::BG_TITLE_BAR, Placement::TOP_LEFT);
            m_arrow = add_filled_triangle(-4_px, 14_px, utils::Color::Name::FG, Placement::TOP_RIGHT);
            m_title = add_text(Position(10_px, 0_px), options.title, utils::Color::Name::FG, Placement::TOP_LEFT);
        }

        void title(std::string text)
        {
            m_title->text(text);
        }

        void toggle_collapsed()
        {
            m_children_visible = !m_children_visible;
            m_arrow->rotation(glm::radians(m_children_visible ? 60.0f : 0.0f));
        }

    private:
        Triangle* m_arrow;
        Text* m_title;

        void on_click(ClickEvent& event) override
        {
            if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS)
                return;
            auto local_cursor_pos = core::Input::cursor_pos_int() - m_actual_pos;
            if (local_cursor_pos.y > core::Application::theme().line_height())
                return;
            event.handled();
            toggle_collapsed();
        }
    };

}
