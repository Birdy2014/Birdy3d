#include "ui/Widget.hpp"

#include "core/Input.hpp"
#include "ui/Canvas.hpp"
#include "ui/Layout.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Triangle.hpp"

namespace Birdy3d::ui {

    Widget::Widget(Options options)
        : options(options) { }

    Rectangle* Widget::add_rectangle(UIVector pos, UIVector size, utils::Color::Name color, Placement placement) {
        std::unique_ptr<Rectangle> rectangle = std::make_unique<Rectangle>(pos, size, color, Shape::OUTLINE, placement);
        Rectangle* ptr = rectangle.get();
        m_shapes.push_back(std::move(rectangle));
        return ptr;
    }

    Rectangle* Widget::add_filled_rectangle(UIVector pos, UIVector size, utils::Color::Name color, Placement placement) {
        std::unique_ptr<Rectangle> rectangle = std::make_unique<Rectangle>(pos, size, color, Shape::FILLED, placement);
        Rectangle* ptr = rectangle.get();
        m_shapes.push_back(std::move(rectangle));
        return ptr;
    }

    Triangle* Widget::add_triangle(UIVector pos, UIVector size, utils::Color::Name color, Placement placement) {
        std::unique_ptr<Triangle> triangle = std::make_unique<Triangle>(pos, size, color, Shape::OUTLINE, placement);
        Triangle* ptr = triangle.get();
        m_shapes.push_back(std::move(triangle));
        return ptr;
    }

    Triangle* Widget::add_filled_triangle(UIVector pos, UIVector size, utils::Color::Name color, Placement placement) {
        std::unique_ptr<Triangle> triangle = std::make_unique<Triangle>(pos, size, color, Shape::FILLED, placement);
        Triangle* ptr = triangle.get();
        m_shapes.push_back(std::move(triangle));
        return ptr;
    }

    Text* Widget::add_text(UIVector pos, std::string text, utils::Color::Name color, Placement placement, float font_size) {
        std::unique_ptr<Text> shape = std::make_unique<Text>(pos, text, color, placement, font_size);
        Text* ptr = shape.get();
        m_shapes.push_back(std::move(shape));
        return ptr;
    }

    void Widget::external_draw() {
        if (options.hidden)
            return;

        // Transform to OpenGL coordinates
        auto viewport_y = core::Application::get_viewport_size().y;
        auto visible_pos_bottom = m_visible_pos.y + m_visible_size.y;
        auto visible_pos_bottom_moved_origin = viewport_y - visible_pos_bottom;

        // Background
        glScissor(m_visible_pos.x, visible_pos_bottom_moved_origin - 1, m_visible_size.x + 2, m_visible_size.y + 2);

        if (m_shapes_visible) {
            for (const auto& s : m_shapes) {
                if (!s->in_foreground)
                    s->draw(m_move);
            }
        }

        // Children
        if (m_children_visible) {
            for (const auto& child : m_children)
                child->external_draw();

            // Foreground
            glScissor(m_visible_pos.x, visible_pos_bottom_moved_origin - 1, m_visible_size.x + 2, m_visible_size.y + 2);
        }

        draw();

        if (m_shapes_visible) {
            for (const auto& s : m_shapes) {
                if (s->in_foreground)
                    s->draw(m_move);
            }
        }
    }

    void Widget::draw() { }

    glm::vec2 Widget::preferred_position(glm::vec2 parentSize, glm::vec2 size) {
        return UIVector::get_relative_position(options.pos, size, parentSize, options.placement);
    }

    glm::vec2 Widget::minimal_size() {
        glm::vec2 children_minsize(m_padding[0] + m_padding[1], m_padding[2] + m_padding[3]);
        if (m_layout && m_children_visible)
            children_minsize += m_layout->minimal_size(m_children);
        return glm::vec2(std::max(children_minsize.x, options.size.x.to_pixels()), std::max(children_minsize.y, options.size.y.to_pixels()));
    }

    glm::vec2 Widget::preferred_size(glm::vec2 parentSize) {
        return glm::max(options.size.to_pixels(parentSize), minimal_size());
    }

    void Widget::arrange(glm::vec2 pos, glm::vec2 size) {
        bool resized = false;
        if (size != m_actual_size)
            resized = true;
        m_actual_pos = pos;
        m_actual_size = size;
        m_move = glm::translate(glm::mat4(1), glm::vec3(m_actual_pos, 0.0f));

        for (const auto& s : m_shapes) {
            s->parent_size(size);
        }

        if (m_layout && m_children_visible)
            m_layout->arrange(m_children, pos + glm::vec2(m_padding[0], m_padding[2]), size - glm::vec2(m_padding[0] + m_padding[1], m_padding[2] + m_padding[3]));

        if (resized)
            on_resize();
    }

    void Widget::set_canvas(Canvas* c) {
        canvas = c;
        for (const auto& child : m_children)
            child->set_canvas(c);
    }

    bool Widget::is_hovering() {
        return canvas->hovering_widget() == this;
    }

    bool Widget::is_focused() {
        return canvas->focused_widget() == this;
    }

    bool Widget::was_last_focused() {
        return canvas->last_focused_widget() == this;
    }

    void Widget::focus() {
        canvas->set_focused(this);
    }

    void Widget::grab_cursor() {
        canvas->set_cursor_grabbed(this, true);
    }

    void Widget::ungrab_cursor() {
        canvas->set_cursor_grabbed(this, false);
    }

    bool Widget::contains(glm::vec2 point) const {
        return point.x > m_visible_pos.x && point.y > m_visible_pos.y && point.x < m_visible_pos.x + m_visible_size.x && point.y < m_visible_pos.y + m_visible_size.y;
    }

    bool Widget::update_hover(bool hover) {
        bool success = false;
        if (options.hidden)
            hover = false;

        // foreground shapes
        if (hover) {
            for (const auto& shape : m_shapes) {
                if (shape->in_foreground && shape->contains(core::Input::cursor_pos() - m_actual_pos)) {
                    canvas->set_hovering(this);
                    return true;
                }
            }
        }

        for (auto it = m_children.rbegin(); it != m_children.rend(); it++) {
            if ((*it)->update_hover(hover && m_children_visible) && m_children_visible) {
                hover = false;
                success = true;
            }
        }

        if (hover) {
            if (contains(core::Input::cursor_pos())) {
                canvas->set_hovering(this);
                return true;
            }
        }

        return success;
    }

    void Widget::update_visible_area(glm::vec2 parent_visible_top_left, glm::vec2 parent_visible_bottom_right) {
        if (options.hidden)
            return;
        m_visible_pos = glm::vec2(std::max(parent_visible_top_left.x, m_actual_pos.x), std::max(parent_visible_top_left.y, m_actual_pos.y));
        glm::vec2 actual_pos2 = m_actual_pos + m_actual_size;
        glm::vec2 visible_pos2 = glm::vec2(std::min(parent_visible_bottom_right.x, actual_pos2.x), std::min(parent_visible_bottom_right.y, actual_pos2.y));
        m_visible_size = visible_pos2 - m_visible_pos;

        for (const auto& child : m_children)
            child->update_visible_area(m_visible_pos, visible_pos2);
    }

    void Widget::on_update() {
        for (auto it = m_children.rbegin(); it != m_children.rend(); it++) {
            (*it)->on_update();
        }
    }

    // TODO: Remove?
    void Widget::late_update() {
        for (auto it = m_children.rbegin(); it != m_children.rend(); it++)
            (*it)->late_update();
    }

    void Widget::add_child(std::shared_ptr<Widget> w) {
        w->parent = this;
        w->set_canvas(canvas);
        m_children.push_back(std::move(w));
    }

    void Widget::clear_children() {
        m_children.clear();
    }

    void Widget::to_foreground(Widget* widget) {
        auto element = std::find_if(m_children.cbegin(), m_children.cend(), [&](const std::shared_ptr<Widget>& w) { return w.get() == widget; });
        if (element == m_children.cend())
            return;
        m_children.splice(m_children.end(), m_children, element);
    }

    void Widget::on_scroll(const events::InputScrollEvent& event) {
        if (parent)
            parent->on_scroll(event);
    }

    void Widget::on_click(const events::InputClickEvent& event) {
        if (parent)
            parent->on_click(event);
    }

    void Widget::on_key(const events::InputKeyEvent& event) {
        if (parent)
            parent->on_key(event);
    }

    void Widget::on_char(const events::InputCharEvent& event) {
        if (parent)
            parent->on_char(event);
    }

    void Widget::add_callback(const std::string& name, CallbackType callback) {
        m_callbacks[name].push_back(callback);
    }

    void Widget::execute_callbacks(const std::string& name, std::any value) {
        for (auto& callback : m_callbacks[name])
            std::invoke(callback, value);
    }

    bool Widget::has_callbacks(const std::string& name) {
        return !m_callbacks[name].empty();
    }

}
