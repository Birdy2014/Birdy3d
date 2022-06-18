#pragma once

#include "ui/Triangle.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d::ui {

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

    /**
     * @brief Widget that displays a Tree.
     *
     * The following callbacks can be added using the Widget::add_callback function:
     * - select: Fires when an item has been selected with any mouse button. Passes a pointer to the selected TreeItem.
     * - select_secundary: Fires when an item has been selected with the right mouse button. Passes a pointer to the selected TreeItem.
     *   This can be used to open a ContextMenu.
     */
    class TreeView : public Widget {
    public:
        bool show_root_item = true;

        TreeView(Options);
        virtual void draw() override;
        virtual bool contains(glm::vec2) const override;
        virtual glm::vec2 minimal_size() override;
        TreeItem& root_item() { return m_root_item; }
        void sync_scene_tree(ecs::Scene*);
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
        virtual bool on_click(const events::InputClickEvent& event) override;

        void update_flat_tree_list();
        void update_flat_tree_list(TreeItem& item, int indent);
    };

}
