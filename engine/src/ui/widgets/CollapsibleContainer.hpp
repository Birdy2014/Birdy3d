#pragma once

#include "core/Input.hpp"
#include "ui/Container.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d::ui {

    class CollapsibleContainer : public Container {
    public:
        struct Options {
            BIRDY3D_WIDGET_OPTIONS_STRUCT
            std::string title;
        };

        CollapsibleContainer(Options options)
            : Container(options)
            , m_title(options.title)
        {
            m_padding = {
                .left = 4_px,
                .right = 4_px,
                .top = 1_em,
                .bottom = 0_px};
        }

        void draw() override
        {
            auto title_bar_color = core::Application::theme().color(utils::Color::Name::BG_TITLE_BAR);
            auto fg_color = core::Application::theme().color(utils::Color::Name::FG);

            paint_background(false);
            paint_rectangle_filled(DimRect::from_position_and_size(0_px, Size{100_pc, 1_em}, Placement::TOP_LEFT), title_bar_color);
            paint_text(Position::make_pixels(10, 0), Placement::TOP_LEFT, m_title);
            paint_triangle_filled(DimRect::from_position_and_size(-4_px, 14_px, Placement::TOP_RIGHT), glm::radians(m_children_visible ? 60.0f : 0.0f), fg_color);
        }

        void title(std::string text)
        {
            m_title.text(text);
        }

        void toggle_collapsed()
        {
            m_children_visible = !m_children_visible;
        }

    private:
        TextDescription m_title;

        void on_click(ClickEvent& event) override
        {
            if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS)
                return;
            auto local_cursor_pos = core::Input::cursor_pos_int() - m_absolute_rect.position();
            if (local_cursor_pos.y > core::Application::theme().line_height())
                return;
            event.handled();
            toggle_collapsed();
        }
    };

}
