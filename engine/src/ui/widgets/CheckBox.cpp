#include "ui/widgets/CheckBox.hpp"

#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d::ui {

    CheckBox::CheckBox(Options options, std::string text)
        : Widget(options) {
        add_rectangle(0_px, 14_px, utils::Color::Name::FG, Placement::CENTER_LEFT);
        m_text_shape = add_text(UIVector(16_px, 0), text, utils::Color::Name::FG, Placement::CENTER_LEFT);
        m_check_shape = add_filled_rectangle(1_px, 10_px, utils::Color::Name::FG, Placement::CENTER_LEFT);
        this->options.size = UIVector(16_px, 0_px) + core::Application::theme().text_renderer().text_size(m_text_shape->text(), core::Application::theme().font_size());
    }

    void CheckBox::draw() {
        m_check_shape->hidden(!checked);
        Widget::draw();
    }

    void CheckBox::on_click(ClickEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS)
            return;

        event.handled();

        checked = !checked;
        if (on_change)
            std::invoke(on_change);
    }

    std::string CheckBox::text() {
        return m_text_shape->text();
    }

    void CheckBox::text(std::string text) {
        m_text_shape->text(text);
        options.size = UIVector(16_px, 0_px) + core::Application::theme().text_renderer().text_size(m_text_shape->text(), core::Application::theme().font_size());
    }

}
