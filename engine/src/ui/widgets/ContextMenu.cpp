#include "ui/widgets/ContextMenu.hpp"

#include "core/Input.hpp"
#include "ui/Canvas.hpp"
#include "ui/Painter.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d::ui {

    ContextItem::ContextItem(std::string text, ClickFunc func)
        : text(TextDescription(text))
        , callback_click(func)
        , m_child_rect(Rect::from_position_and_size(glm::ivec2(0), glm::ivec2(m_padding * 2)))
    { }

    ContextItem& ContextItem::add_child(std::string text, ClickFunc func)
    {
        m_child_rect.height(m_child_rect.height() + (1_em).to_pixels());
        return children.emplace_back(text, func);
    }

    void ContextItem::remove_child(std::string text)
    {
        auto it = std::remove_if(children.begin(), children.end(), [&text](ContextItem const& item) {
            return text == item.text.text();
        });
        if (it != children.end())
            m_child_rect.height(m_child_rect.height() - (1_em).to_pixels());
    }

    ContextMenu::ContextMenu(Options options)
        : Widget(options)
        , root_item(ContextItem("Root", nullptr))
    {
        m_arrow_size = core::Application::theme().font_size() / 2;
        m_half_arrow_size = core::Application::theme().font_size() / 4;
        this->hidden = true;
        m_children_visible = false;
    }

    void ContextMenu::draw()
    {
        root_item.m_child_rect.position(m_absolute_rect.position());
        glDisable(GL_SCISSOR_TEST);
        draw_context_item_children(root_item);
        glEnable(GL_SCISSOR_TEST);
    }

    void ContextMenu::open(glm::ivec2 open_pos)
    {
        position = Position::make_pixels(open_pos);
        m_absolute_rect.position(open_pos);
        hidden = false;
        focus();
        canvas->to_foreground(this);
        for (auto& child_item : root_item.children)
            child_item.opened = false;
    }

    void ContextMenu::open()
    {
        auto open_pos = core::Input::cursor_pos_int();
        glm::ivec2 viewport = core::Application::get_viewport_size();
        if (open_pos.x + root_item.m_child_rect.width() > viewport.x)
            position.x = Dimension::make_pixels(open_pos.x - root_item.m_child_rect.width()); // Left
        else
            position.x = Dimension::make_pixels(open_pos.x); // Right
        if (open_pos.y + root_item.m_child_rect.height() > viewport.y)
            position.y = Dimension::make_pixels(open_pos.y - root_item.m_child_rect.height()); // Up
        else
            position.y = Dimension::make_pixels(open_pos.y); // Down
        open(position.to_pixels());
    }

    void ContextMenu::on_update()
    {
        if (hidden)
            return;

        handle_context_item_children_click(root_item, false);
    }

    bool ContextMenu::contains(glm::ivec2 point) const
    {
        return context_item_contains(root_item, point);
    }

    void ContextMenu::draw_context_item_children(ContextItem& item)
    {
        // TODO: Put the x size calculation in the ContextItem and figure out how to add the arrow length after children are added.
        item.m_child_rect.size(glm::ivec2{0, item.m_child_rect.height()});
        for (auto const& child_item : item.children) {
            auto text_width = child_item.text.text_size().x;
            if (!child_item.children.empty())
                text_width += m_arrow_size + 5;
            if (item.m_child_rect.width() < text_width)
                item.m_child_rect.size(glm::ivec2{text_width, item.m_child_rect.height()});
        }
        item.m_child_rect.width(item.m_child_rect.width() + item.m_padding * 2);

        // TODO: only get the color once?
        auto bg_color = core::Application::theme().color(utils::Color::Name::BG);
        auto border_color = core::Application::theme().color(utils::Color::Name::BORDER);
        auto fg_color = core::Application::theme().color(utils::Color::Name::FG);
        Painter::the().paint_rectangle_filled(item.m_child_rect, bg_color, 1, border_color);

        auto offset_y = item.m_padding;
        for (auto const& child_item : item.children) {
            if (!child_item.children.empty()) {
                auto position = item.m_child_rect.position() + glm::ivec2(item.m_child_rect.width() - m_arrow_size - item.m_padding, offset_y + (0.5_em).to_pixels() - m_half_arrow_size);
                Painter::the().paint_triangle_filled(Rect::from_position_and_size(position, glm::ivec2{m_arrow_size}), glm::radians(30.0f), fg_color);
            }
            auto position = glm::ivec2(item.m_child_rect.left() + item.m_padding, item.m_child_rect.top() + offset_y);
            Painter::the().paint_text(position, child_item.text);
            offset_y += (1_em).to_pixels();
        }
        for (auto& child_item : item.children) {
            if (!child_item.children.empty() && child_item.opened) {
                draw_context_item_children(child_item);
                break;
            }
        }
    }

    bool ContextMenu::handle_context_item_children_click(ContextItem& item, bool click)
    {
        auto local_pos = core::Input::cursor_pos_int() - item.m_child_rect.position();
        bool found = false;
        if (local_pos.x > 0 && local_pos.x < item.m_child_rect.width() && local_pos.y > 0 && local_pos.y < item.m_child_rect.height()) {
            auto offset_y = item.m_padding;
            for (auto& child_item : item.children) {
                offset_y += (1_em).to_pixels();
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
                        auto viewport = core::Application::get_viewport_size();
                        if (item.m_child_rect.left() + child_item.m_child_rect.width() > viewport.x)
                            child_item.m_child_rect.left(item.m_child_rect.left() - child_item.m_child_rect.width());
                        else
                            child_item.m_child_rect.left(item.m_child_rect.left() + item.m_child_rect.width()); // Right
                        if (item.m_child_rect.top() + offset_y + child_item.m_child_rect.height() - (1_em).to_pixels() - item.m_padding > viewport.y)
                            child_item.m_child_rect.top(item.m_child_rect.top() + offset_y - child_item.m_child_rect.height() - (1_em).to_pixels() - item.m_padding); // Up
                        else
                            child_item.m_child_rect.top(item.m_child_rect.top() + offset_y - (1_em).to_pixels() - item.m_padding); // Down

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

    bool ContextMenu::context_item_contains(ContextItem const& item, glm::ivec2 point) const
    {
        if (point.x > item.m_child_rect.left() && point.y > item.m_child_rect.top() && point.x < item.m_child_rect.left() + item.m_child_rect.width() && point.y < item.m_child_rect.top() + item.m_child_rect.height())
            return true;
        for (auto const& child_item : item.children) {
            if (child_item.opened && !child_item.children.empty()) {
                if (context_item_contains(child_item, point))
                    return true;
            }
        }
        return false;
    }

    void ContextMenu::on_click(ClickEvent& event)
    {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS)
            return;

        if (handle_context_item_children_click(root_item, true)) {
            event.handled();
        } else {
            hidden = true;
        }
    }

    void ContextMenu::on_key(KeyEvent& event)
    {
        event.handled();

        hidden = true;
    }

    void ContextMenu::on_focus_lost(FocusLostEvent&)
    {
        hidden = true;
    }

    MenuBar::MenuBar(Options options)
        : Widget(options)
    { }

    ContextItem& MenuBar::add_item(std::string text)
    {
        auto menu = std::make_unique<ContextMenu>(Options{});
        menu->canvas = canvas;
        menu->root_item.text.text(text);
        ContextItem& item = menu->root_item;
        m_menus.push_back(std::move(menu));
        return item;
    }

    void MenuBar::remove_item(std::string text)
    {
        std::remove_if(m_menus.begin(), m_menus.end(), [&text](std::unique_ptr<ContextMenu> const& menu) {
            return text == menu->root_item.text.text();
        });
    }

    void MenuBar::draw()
    {
        paint_background(false);

        int x = 0;
        for (auto& menu : m_menus) {
            paint_text(glm::ivec2(x, 0), menu->root_item.text);
            // menu->external_draw() would reset glScissor, but menu->draw() doesn't check for hidden.
            if (!menu->hidden)
                menu->draw();
            x += menu->root_item.text.text_size().x + m_menu_gap;
        }
    }

    void MenuBar::on_click(ClickEvent& event)
    {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS)
            return;
        int curosr_x = core::Input::cursor_pos().x - m_absolute_rect.left();
        int x = 0;
        for (auto& menu : m_menus) {
            x += menu->root_item.text.text_size().x;
            if (curosr_x < x) {
                if (menu->was_last_focused())
                    return;
                glm::ivec2 open_pos = m_absolute_rect.position() + glm::ivec2(x - menu->root_item.text.text_size().x, m_absolute_rect.height());
                menu->open(open_pos);
                return;
            }
            x += m_menu_gap;
            if (curosr_x < x)
                return;
        }
    }

}
