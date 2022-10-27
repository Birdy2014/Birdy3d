#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    class ContextItem {
    public:
        typedef std::function<void()> ClickFunc;

        std::unique_ptr<Text> text;
        std::list<ContextItem> children;
        ClickFunc callback_click;
        bool opened = false;

        ContextItem(std::string, ClickFunc);
        ContextItem& add_child(std::string, ClickFunc = nullptr);
        void remove_child(std::string);

    private:
        friend class ContextMenu;
        friend class MenuBar;

        Dimension m_padding = 10_px;
        Position m_child_rect_pos;
        Size m_child_rect_size;
    };

    class ContextMenu : public Widget {
    public:
        ContextItem root_item;

        ContextMenu(Options);
        void draw() override;
        void open(glm::ivec2);
        void open();
        void on_update() override;

    private:
        int m_arrow_size;
        Rectangle* m_background_rect;
        Rectangle* m_border_rect;
        Triangle* m_submenu_triangle;

        [[nodiscard]] bool contains(glm::ivec2) const override;

        void draw_context_item_children(ContextItem&);
        bool handle_context_item_children_click(ContextItem&, bool click);
        [[nodiscard]] bool context_item_contains(ContextItem const&, Position) const;

        void on_click(ClickEvent&) override;
        void on_key(KeyEvent&) override;
        void on_focus_lost(FocusLostEvent&) override;
    };

    class MenuBar : public Widget {
    public:
        MenuBar(Options);
        ContextItem& add_item(std::string);
        void remove_item(std::string);
        void draw() override;

    private:
        std::vector<std::unique_ptr<ContextMenu>> m_menus;
        int m_menu_gap = 20;

        void on_click(ClickEvent&) override;
    };

}
