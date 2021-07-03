#include "ui/Widget.hpp"

#include "core/Input.hpp"
#include "ui/Canvas.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Triangle.hpp"

namespace Birdy3d {

    Widget::Widget(UIVector pos, UIVector size, Placement placement, Theme* theme, std::string name)
        : name(name)
        , pos(pos)
        , size(size)
        , placement(placement)
        , theme(theme) { }

    Widget::~Widget() {
        for (Shape* s : shapes) {
            delete s;
        }
    }

    Rectangle* Widget::addRectangle(UIVector pos, UIVector size, Color color, Placement placement) {
        Rectangle* rectangle = new Rectangle(pos, size, color, Shape::OUTLINE, placement);
        this->shapes.push_back(rectangle);
        return rectangle;
    }

    Rectangle* Widget::addFilledRectangle(UIVector pos, UIVector size, Color color, Placement placement) {
        Rectangle* rectangle = new Rectangle(pos, size, color, Shape::FILLED, placement);
        this->shapes.push_back(rectangle);
        return rectangle;
    }

    Triangle* Widget::addTriangle(UIVector pos, UIVector size, Color color) {
        Triangle* triangle = new Triangle(pos, size, color, Shape::OUTLINE);
        this->shapes.push_back(triangle);
        return triangle;
    }

    Triangle* Widget::addFilledTriangle(UIVector pos, UIVector size, Color color) {
        Triangle* triangle = new Triangle(pos, size, color, Shape::FILLED);
        this->shapes.push_back(triangle);
        return triangle;
    }

    Text* Widget::addText(UIVector pos, float fontSize, std::string text, Color color, Placement placement) {
        Text* shape = new Text(pos, fontSize, text, color, placement, theme->text_renderer());
        this->shapes.push_back(shape);
        return shape;
    }

    void Widget::draw() {
        if (hidden)
            return;

        // FIXME: the -1 and +2 should not be necessary and don't completely fix the problem, that widget borders are sometimes invisible.
        glScissor(actualPos.x - 1, actualPos.y - 1, actualSize.x + 2, actualSize.y + 2);

        glm::mat4 move = normalizedMove();

        for (Shape* s : this->shapes) {
            s->draw(move);
        }
    }

    glm::vec2 Widget::preferredPosition(glm::vec2 parentSize, glm::vec2 size) {
        return Utils::getRelativePosition(this->pos, size, parentSize, this->placement);
    }

    glm::vec2 Widget::minimalSize() {
        return size.toPixels();
    }

    glm::vec2 Widget::preferredSize(glm::vec2 parentSize) {
        return glm::max(size.toPixels(parentSize), minimalSize());
    }

    void Widget::arrange(glm::vec2 pos, glm::vec2 size) {
        this->actualPos = pos;
        this->actualSize = size;

        for (Shape* s : shapes) {
            s->parentSize(size);
        }
    }

    void Widget::set_canvas(Canvas* c) {
        canvas = c;
    }

    glm::mat4 Widget::normalizedMove() {
        glm::vec2 viewportSize = Application::getViewportSize();
        glm::mat4 move = glm::ortho(0.0f, viewportSize.x, 0.0f, viewportSize.y);
        return glm::translate(move, glm::vec3(actualPos, 0.0f));
    }

    bool Widget::is_hovering() {
        return canvas->m_hovering_widget == this;
    }

    bool Widget::is_focused() {
        return canvas->m_focused_widget == this;
    }

    void Widget::focus() {
        if (canvas->m_focused_widget)
            canvas->m_focused_widget->on_focus_lost();
        canvas->m_focused_widget = this;
        canvas->m_cursor_grabbed = false;
        canvas->m_focused_widget->on_focus();
    }

    void Widget::grab_cursor() {
        canvas->m_cursor_grabbed = true;
    }

    void Widget::ungrab_cursor() {
        canvas->m_cursor_grabbed = false;
    }

    bool Widget::update_hover(bool hover) {
        if (hidden)
            hover = false;
        if (hover) {
            glm::vec2 cursorPos = Input::cursorPos();
            if (cursorPos.x > actualPos.x && cursorPos.y > actualPos.y && cursorPos.x < actualPos.x + actualSize.x && cursorPos.y < actualPos.y + actualSize.y) {
                canvas->m_hovering_widget = this;
                return true;
            }
        }
        if (m_hovered_last_frame) {
            on_mouse_leave();
            m_hovered_last_frame = false;
        }
        return false;
    }

    void Widget::late_update() {
        if (is_hovering() && !m_hovered_last_frame) {
            on_mouse_enter();
            m_hovered_last_frame = true;
        }
    }

}
