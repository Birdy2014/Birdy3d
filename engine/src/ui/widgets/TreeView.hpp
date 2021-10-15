#pragma once

#include "ui/Triangle.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d {

    class ContextMenu;
    class Scene;
    class TreeView;

    class TreeItem {
    public:
        std::unique_ptr<Text> text;
        std::list<TreeItem> children;
        bool collapsed = false;
        std::any data;

        TreeItem(std::string, TreeView*);
        TreeItem& add_child(std::string text);
        void remove_child(const TreeItem*);

    private:
        friend class TreeView;

        TreeView* m_treeview;
        std::unique_ptr<Triangle> m_collapse_button;

        void set_treeview(TreeView* treeview);
    };

    class TreeView : public Widget {
    public:
        std::function<void(TreeItem&)> callback_select;
        std::weak_ptr<ContextMenu> context_menu;

        TreeView(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), Placement placement = Placement::BOTTOM_LEFT);
        virtual void draw() override;
        virtual glm::vec2 minimal_size() override;
        TreeItem& root_item() { return m_root_item; }
        void sync_scene_tree(Scene*);
        void selected_item(TreeItem* item) { m_selected_item = item; }

    protected:
        friend class TreeItem;

        const int m_indent_size = 20;
        const int m_offset_x_button = -12;
        const int m_offset_x_left = 15;
        TreeItem m_root_item;
        std::list<std::pair<int, TreeItem&>> m_flat_tree_list;
        bool m_items_changed = true;
        Rectangle* m_item_highlight_rect;
        TreeItem* m_selected_item = nullptr;

        virtual void on_update() override;
        virtual void on_click(InputClickEvent* event) override;
        virtual void on_key(InputKeyEvent* event) override;
        virtual void on_char(InputCharEvent* event) override;
        virtual void on_mouse_enter() override;
        virtual void on_mouse_leave() override;
        virtual void on_focus_lost() override;

        void update_flat_tree_list();
        void update_flat_tree_list(TreeItem& item, int indent);
    };

}
