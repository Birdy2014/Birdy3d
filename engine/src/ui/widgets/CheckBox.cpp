#include "ui/widgets/CheckBox.hpp"

#include "ui/Theme.hpp"

namespace Birdy3d::ui {

    CheckBox::CheckBox(Options options)
        : Widget(options)
        , m_text(options.text)
    {
        size = Size(16_px, 0_px) + Size::make_pixels(m_text.text_size());
    }

    void CheckBox::draw()
    {
        auto fg_color = core::Application::theme().color(utils::Color::Name::FG);
        paint_rectangle_filled(DimRect::from_position_and_size(0_px, 14_px, Placement::CENTER_LEFT), utils::Color::NONE, 1, fg_color);
        paint_text(Position::make_pixels(16, 0), Placement::CENTER_LEFT, m_text);
        if (checked) {
            paint_rectangle_filled(DimRect::from_position_and_size(1_px, 10_px, Placement::CENTER_LEFT), fg_color);
        }
    }

    void CheckBox::on_click(ClickEvent& event)
    {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS)
            return;

        event.handled();

        checked = !checked;
        if (on_change)
            std::invoke(on_change);
    }

    std::string CheckBox::text()
    {
        return m_text.text();
    }

    void CheckBox::text(std::string text)
    {
        m_text.text(text);
        size = Size(16_px, 0_px) + Size::make_pixels(m_text.text_size());
    }

}
