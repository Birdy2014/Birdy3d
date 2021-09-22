#include "ui/widgets/ContextMenu.hpp"

#include "core/Input.hpp"
#include "ui/Canvas.hpp"
#include "ui/DirectionalLayout.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Triangle.hpp"

namespace Birdy3d {

    ContextItem::ContextItem(std::string text, ClickFunc func)
        : text(text)
        , callback_click(func) { }

    ContextItem& ContextItem::add_child(std::string text, ClickFunc func) {
        return children.emplace_back(text, func);
    }

    ContextMenu::ContextMenu()
        : Widget()
        , root_item(ContextItem("Root", nullptr)) {
        m_arrow_size = Application::theme->font_size / 2;
        m_background_rect = add_filled_rectangle(0_px, 0_px, Application::theme->color_bg, Placement::BOTTOM_LEFT);
        m_border_rect = add_rectangle(0_px, 0_px, Application::theme->color_border, Placement::BOTTOM_LEFT);
        m_submenu_triangle = add_filled_triangle(0_px, Unit(m_arrow_size), Application::theme->color_fg);
        hidden = true;
    }

    void ContextMenu::draw() {
        if (hidden)
            return;
        root_item.m_child_rect_pos = m_actual_pos;
        draw_context_item_children(root_item);
    }

    void ContextMenu::open() {
        glm::vec2 open_pos = Input::cursor_pos();
        glm::vec2 viewport = Application::get_viewport_size();
        if (open_pos.x + root_item.m_child_rect_size.x > viewport.x)
            pos.x = open_pos.x - root_item.m_child_rect_size.x; // Left
        else
            pos.x = open_pos.x; // Right
        if (open_pos.y - root_item.m_child_rect_size.y < 0)
            pos.y = open_pos.y; // Up
        else
            pos.y = open_pos.y - root_item.m_child_rect_size.y; // Down
        hidden = false;
        focus();
        canvas->to_foreground(this);
        for (auto& child_item : root_item.children)
            child_item.opened = false;
    }

    void ContextMenu::on_update() {
        if (hidden)
            return;

        handle_context_item_children_click(root_item, false);
    }

    bool ContextMenu::contains(glm::vec2 point) const {
        return context_item_contains(root_item, point);
    }

    void ContextMenu::draw_context_item_children(ContextItem& item) {
        item.m_child_rect_size = glm::vec2(0);
        for (const auto& child_item : item.children) {
            item.m_child_rect_size.y += Application::theme->line_height;
            float text_width = Application::theme->text_renderer()->text_size(child_item.text, Application::theme->font_size).x;
            if (!child_item.children.empty())
                text_width += m_arrow_size + 5;
            if (item.m_child_rect_size.x < text_width)
                item.m_child_rect_size.x = text_width;
        }
        item.m_child_rect_size += glm::vec2(m_padding * 2);

        m_background_rect->position(item.m_child_rect_pos);
        m_background_rect->size(item.m_child_rect_size);
        m_background_rect->draw(glm::mat4(1));
        m_border_rect->position(item.m_child_rect_pos);
        m_border_rect->size(item.m_child_rect_size);
        m_border_rect->draw(glm::mat4(1));

        int offset_y = item.m_child_rect_size.y - m_padding;
        for (const auto& child_item : item.children) {
            offset_y -= Application::theme->line_height;
            if (!child_item.children.empty()) {
                m_submenu_triangle->position(item.m_child_rect_pos + glm::vec2(item.m_child_rect_size.x - m_arrow_size - m_padding, offset_y + (Application::theme->line_height - m_arrow_size) / 2));
                m_submenu_triangle->rotation(glm::radians(30.0f));
                m_submenu_triangle->draw(glm::mat4(1));
            }
            Application::theme->text_renderer()->render_text(child_item.text, item.m_child_rect_pos.x + m_padding, item.m_child_rect_pos.y + offset_y, Application::theme->font_size, Application::theme->color_fg);
        }
        for (auto& child_item : item.children) {
            if (!child_item.children.empty() && child_item.opened) {
                draw_context_item_children(child_item);
                break;
            }
        }
    }

    bool ContextMenu::handle_context_item_children_click(ContextItem& item, bool click) {
        glm::vec2 local_pos = Input::cursor_pos() - item.m_child_rect_pos;
        bool found = false;
        if (local_pos.x > 0 && local_pos.x < item.m_child_rect_size.x && local_pos.y > 0 && local_pos.y < item.m_child_rect_size.y) {
            int offset_y = item.m_child_rect_size.y - m_padding;
            for (auto& child_item : item.children) {
                offset_y -= Application::theme->line_height;
                if (!found) {
                    if (offset_y < local_pos.y) {
                        if (click && child_item.children.empty()) {
                            if (child_item.callback_click)
                                child_item.callback_click();
                            hidden = true;
                            return true;
                        }
                        if (!child_item.children.empty()) {
                            child_item.opened = true;
                            // Set position of new menu
                            glm::vec2 viewport = Application::get_viewport_size();
                            if (item.m_child_rect_pos.x + child_item.m_child_rect_size.x > viewport.x)
                                child_item.m_child_rect_pos.x = item.m_child_rect_pos.x - child_item.m_child_rect_size.x; // Left
                            else
                                child_item.m_child_rect_pos.x = item.m_child_rect_pos.x + item.m_child_rect_size.x; // Right
                            if (item.m_child_rect_pos.y + offset_y - child_item.m_child_rect_size.y + Application::theme->line_height + m_padding < 0)
                                child_item.m_child_rect_pos.y = item.m_child_rect_pos.y + offset_y - m_padding; // Up
                            else
                                child_item.m_child_rect_pos.y = item.m_child_rect_pos.y + offset_y - child_item.m_child_rect_size.y + Application::theme->line_height + m_padding; // Down

                            // Close all children of newly opened item
                            for (auto& child_child_item : child_item.children)
                                child_child_item.opened = false;
                        }
                        found = true;
                        continue;
                    }
                }
                child_item.opened = false;
            }
        }

        if (found)
            return true;

        for (auto& child_item : item.children) {
            if (!child_item.children.empty() && child_item.opened) {
                if (handle_context_item_children_click(child_item, click))
                    found = true;
            }
        }

        return found;
    }

    bool ContextMenu::context_item_contains(const ContextItem& item, glm::vec2 point) const {
        if (point.x > item.m_child_rect_pos.x && point.y > item.m_child_rect_pos.y && point.x < item.m_child_rect_pos.x + item.m_child_rect_size.x && point.y < item.m_child_rect_pos.y + item.m_child_rect_size.y)
            return true;
        for (const auto& child_item : item.children) {
            if (child_item.opened && !child_item.children.empty()) {
                if (context_item_contains(child_item, point))
                    return true;
            }
        }
        return false;
    }

    void ContextMenu::on_click(InputClickEvent* event) {
        if (event->button != GLFW_MOUSE_BUTTON_LEFT || event->action != GLFW_PRESS)
            return;

        if (!handle_context_item_children_click(root_item, true))
            hidden = true;
    }

    void ContextMenu::on_key(InputKeyEvent* event) {
        hidden = true;
    }

    void ContextMenu::on_focus_lost() {
        hidden = true;
    }

}
