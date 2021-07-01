#include "ui/widgets/CheckBox.hpp"

#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    CheckBox::CheckBox(UIVector pos, Placement placement, std::string text)
        : Widget(pos, 0_px, placement) {
        addRectangle(0_px, 14_px, theme->color_fg, Placement::CENTER_LEFT);
        m_text_shape = addText(UIVector(16_px, 0), theme->fontSize, text, theme->color_fg, Placement::CENTER_LEFT);
        m_check_shape = addFilledRectangle(1_px, 10_px, theme->color_fg, Placement::CENTER_LEFT);
        size = UIVector(16_px, 0_px) + theme->text_renderer()->textSize(m_text_shape->text, theme->fontSize);
    }

    void CheckBox::draw() {
        m_check_shape->hidden(!checked);
        Widget::draw();
    }

    bool CheckBox::onClick(InputClickEvent* event, bool hover) {
        if (!hover || event->button != GLFW_MOUSE_BUTTON_LEFT || event->action != GLFW_PRESS)
            return true;

        checked = !checked;
        if (change_callback)
            change_callback(checked);
        return true;
    }

    std::string CheckBox::text() {
        return m_text_shape->text;
    }

    void CheckBox::text(std::string text) {
        m_text_shape->text = text;
        size = UIVector(16_px, 0_px) + theme->text_renderer()->textSize(m_text_shape->text, theme->fontSize);
    }

}
