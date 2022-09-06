#include "ui/widgets/ContextMenu.hpp"

#include "core/Input.hpp"
#include "ui/Canvas.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Triangle.hpp"

namespace Birdy3d::ui {

    ContextItem::ContextItem(std::string text, ClickFunc func)
        : text(std::make_unique<Text>(0_px, text, utils::Color::Name::FG, Placement::TOP_LEFT))
        , callback_click(func)
        , m_child_rect_size(glm::vec2(m_padding * 2)) { }

    ContextItem& ContextItem::add_child(std::string text, ClickFunc func) {
        m_child_rect_size.y += core::Application::theme().line_height();
        return children.emplace_back(text, func);
    }

    void ContextItem::remove_child(std::string text) {
        auto it = std::remove_if(children.begin(), children.end(), [&text](const ContextItem& item) {
            return text == item.text->text();
        });
        if (it != children.end())
            m_child_rect_size.y -= core::Application::theme().line_height();
    }

    ContextMenu::ContextMenu(Options options)
        : Widget(options)
        , root_item(ContextItem("Root", nullptr)) {
        m_arrow_size = core::Application::theme().font_size() / 2;
        m_background_rect = add_filled_rectangle(0_px, 0_px, utils::Color::Name::BG, Placement::TOP_LEFT);
        m_border_rect = add_rectangle(0_px, 0_px, utils::Color::Name::BORDER, Placement::TOP_LEFT);
        m_submenu_triangle = add_filled_triangle(0_px, Unit(m_arrow_size), utils::Color::Name::FG);
        this->hidden = true;
        m_children_visible = false;
        m_shapes_visible = false;
    }

    void ContextMenu::draw() {
        root_item.m_child_rect_pos = m_actual_pos;
        glDisable(GL_SCISSOR_TEST);
        draw_context_item_children(root_item);
        glEnable(GL_SCISSOR_TEST);
    }

    void ContextMenu::open(glm::vec2 open_pos) {
        position = open_pos;
        m_actual_pos = position;
        hidden = false;
        focus();
        canvas->to_foreground(this);
        for (auto& child_item : root_item.children)
            child_item.opened = false;
    }

    void ContextMenu::open() {
        glm::vec2 open_pos = core::Input::cursor_pos();
        glm::vec2 viewport = core::Application::get_viewport_size();
        if (open_pos.x + root_item.m_child_rect_size.x > viewport.x)
            position.x = open_pos.x - root_item.m_child_rect_size.x; // Left
        else
            position.x = open_pos.x; // Right
        if (open_pos.y + root_item.m_child_rect_size.y > viewport.y)
            position.y = open_pos.y - root_item.m_child_rect_size.y; // Up
        else
            position.y = open_pos.y; // Down
        open(position);
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
        // TODO: Put the x size calculation in the ContextItem and figure out how to add the arrow length after children are added.
        item.m_child_rect_size.x = 0;
        for (const auto& child_item : item.children) {
            float text_width = child_item.text->size().x;
            if (!child_item.children.empty())
                text_width += m_arrow_size + 5;
            if (item.m_child_rect_size.x < text_width)
                item.m_child_rect_size.x = text_width;
        }
        item.m_child_rect_size.x += item.m_padding * 2;

        m_background_rect->position(item.m_child_rect_pos);
        m_background_rect->size(item.m_child_rect_size);
        m_background_rect->draw(glm::mat4(1));
        m_border_rect->position(item.m_child_rect_pos);
        m_border_rect->size(item.m_child_rect_size);
        m_border_rect->draw(glm::mat4(1));

        int offset_y = item.m_padding;
        for (const auto& child_item : item.children) {
            if (!child_item.children.empty()) {
                m_submenu_triangle->position(item.m_child_rect_pos + glm::vec2(item.m_child_rect_size.x - m_arrow_size - item.m_padding, offset_y + (core::Application::theme().line_height() - m_arrow_size) / 2));
                m_submenu_triangle->rotation(glm::radians(30.0f));
                m_submenu_triangle->draw(glm::mat4(1));
            }
            child_item.text->position(UIVector(item.m_child_rect_pos.x + item.m_padding, item.m_child_rect_pos.y + offset_y));
            child_item.text->draw(glm::mat4(1));
            offset_y += core::Application::theme().line_height();
        }
        for (auto& child_item : item.children) {
            if (!child_item.children.empty() && child_item.opened) {
                draw_context_item_children(child_item);
                break;
            }
        }
    }

    bool ContextMenu::handle_context_item_children_click(ContextItem& item, bool click) {
        glm::vec2 local_pos = core::Input::cursor_pos() - item.m_child_rect_pos;
        bool found = false;
        if (local_pos.x > 0 && local_pos.x < item.m_child_rect_size.x && local_pos.y > 0 && local_pos.y < item.m_child_rect_size.y) {
            int offset_y = item.m_padding;
            for (auto& child_item : item.children) {
                offset_y += core::Application::theme().line_height();
                child_item.opened = false;
                if (found)
                    continue;
                if (local_pos.y < offset_y) {
                    if (click && child_item.children.empty()) {
                        if (child_item.callback_click)
                            child_item.callback_click();
                        hidden = true;
                        return true;
                    }
                    if (!child_item.children.empty()) {
                        child_item.opened = true;
                        // Set position of new menu
                        glm::vec2 viewport = core::Application::get_viewport_size();
                        if (item.m_child_rect_pos.x + child_item.m_child_rect_size.x > viewport.x)
                            child_item.m_child_rect_pos.x = item.m_child_rect_pos.x - child_item.m_child_rect_size.x; // Left
                        else
                            child_item.m_child_rect_pos.x = item.m_child_rect_pos.x + item.m_child_rect_size.x; // Right
                        if (item.m_child_rect_pos.y + offset_y + child_item.m_child_rect_size.y - core::Application::theme().line_height() - item.m_padding > core::Application::get_viewport_size().y)
                            child_item.m_child_rect_pos.y = item.m_child_rect_pos.y + offset_y - child_item.m_child_rect_size.y - core::Application::theme().line_height() - item.m_padding; // Up
                        else
                            child_item.m_child_rect_pos.y = item.m_child_rect_pos.y + offset_y - core::Application::theme().line_height() - item.m_padding; // Down

                        // Close all children of newly opened item
                        for (auto& child_child_item : child_item.children)
                            child_child_item.opened = false;
                    }
                    found = true;
                }
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

    void ContextMenu::on_click(ClickEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS)
            return;

        if (handle_context_item_children_click(root_item, true)) {
            event.handled();
        } else {
            hidden = true;
        }
    }

    void ContextMenu::on_key(KeyEvent& event) {
        event.handled();

        hidden = true;
    }

    void ContextMenu::on_focus_lost(FocusLostEvent&) {
        hidden = true;
    }

    MenuBar::MenuBar(Options options)
        : Widget(options) {
        add_filled_rectangle(0_p, 100_p, utils::Color::Name::BG);
    }

    ContextItem& MenuBar::add_item(std::string text) {
        auto menu = std::make_unique<ContextMenu>(Options {});
        menu->canvas = canvas;
        menu->root_item.text->text(text);
        ContextItem& item = menu->root_item;
        m_menus.push_back(std::move(menu));
        return item;
    }

    void MenuBar::remove_item(std::string text) {
        std::remove_if(m_menus.begin(), m_menus.end(), [&text](const std::unique_ptr<ContextMenu>& menu) {
            return text == menu->root_item.text->text();
        });
    }

    void MenuBar::draw() {
        Widget::draw();
        int x = 0;
        for (auto& menu : m_menus) {
            menu->root_item.text->position(UIVector(x, 0));
            menu->root_item.text->draw(m_move);
            // menu->external_draw() would reset glScissor, but menu->draw() doesn't check for hidden.
            if (!menu->hidden)
                menu->draw();
            x += menu->root_item.text->size().x + m_menu_gap;
        }
    }

    void MenuBar::on_click(ClickEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS)
            return;
        int curosr_x = core::Input::cursor_pos().x - m_actual_pos.x;
        int x = 0;
        for (auto& menu : m_menus) {
            x += menu->root_item.text->size().x;
            if (curosr_x < x) {
                if (menu->was_last_focused())
                    return;
                glm::vec2 open_pos = m_actual_pos + glm::vec2(x - menu->root_item.text->size().x, m_actual_size.y);
                menu->open(open_pos);
                return;
            }
            x += m_menu_gap;
            if (curosr_x < x)
                return;
        }
    }

}
