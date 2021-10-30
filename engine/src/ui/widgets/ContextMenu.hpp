#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class ContextItem {
    public:
        typedef std::function<void()> ClickFunc;

        std::unique_ptr<Text> text;
        std::list<ContextItem> children;
        ClickFunc callback_click;
        bool opened = false;

        ContextItem(std::string, ClickFunc);
        ContextItem& add_child(std::string, ClickFunc = nullptr);

    private:
        friend class ContextMenu;

        glm::vec2 m_child_rect_pos;
        glm::vec2 m_child_rect_size;
    };

    class ContextMenu : public Widget {
    public:
        ContextItem root_item;

        ContextMenu();
        void draw() override;
        void open();
        void on_update() override;

    private:
        int m_padding = 10;
        int m_arrow_size;
        Rectangle* m_background_rect;
        Rectangle* m_border_rect;
        Triangle* m_submenu_triangle;

        bool contains(glm::vec2) const override;

        void draw_context_item_children(ContextItem&);
        bool handle_context_item_children_click(ContextItem&, bool click);
        bool context_item_contains(const ContextItem&, glm::vec2) const;

        void on_click(const InputClickEvent& event) override;
        void on_key(const InputKeyEvent& event) override;
        void on_focus_lost() override;
    };

}
