#include "ui/widgets/CheckBox.hpp"

#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    CheckBox::CheckBox(UIVector pos, Placement placement, std::string text)
        : Widget(pos, 0_px, placement) {
        add_rectangle(0_px, 14_px, Application::theme->color_fg, Placement::CENTER_LEFT);
        m_text_shape = add_text(UIVector(16_px, 0), Application::theme->font_size, text, Application::theme->color_fg, Placement::CENTER_LEFT);
        m_check_shape = add_filled_rectangle(1_px, 10_px, Application::theme->color_fg, Placement::CENTER_LEFT);
        size = UIVector(16_px, 0_px) + Application::theme->text_renderer()->textSize(m_text_shape->text, Application::theme->font_size);
    }

    void CheckBox::draw() {
        m_check_shape->hidden(!checked);
        Widget::draw();
    }

    void CheckBox::on_click(InputClickEvent* event) {
        if (event->button != GLFW_MOUSE_BUTTON_LEFT || event->action != GLFW_PRESS)
            return;

        checked = !checked;
        if (callback_change)
            callback_change(checked);
        return;
    }

    std::string CheckBox::text() {
        return m_text_shape->text;
    }

    void CheckBox::text(std::string text) {
        m_text_shape->text = text;
        size = UIVector(16_px, 0_px) + Application::theme->text_renderer()->textSize(m_text_shape->text, Application::theme->font_size);
    }

}
