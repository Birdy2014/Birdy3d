#pragma once

#include "ui/Widget.hpp"
#include <memory>

namespace Birdy3d::ui {

    class TreeView;
    class TreeModel;

    class TreeItem {
    public:
        std::string text;
        std::any data;
        std::size_t local_index;
        std::size_t depth = 0;
        bool is_leaf;
        std::size_t hash;
    };

    struct TreeItemPosition {
        TreeItem& item;
        TreeItem& parent;
        std::size_t local_index;
    };

    class TreeModel {
    public:
        virtual ~TreeModel() = default;

        virtual std::optional<TreeItem> item(TreeItem const* parent, std::size_t local_index) = 0;
        virtual void move_item(TreeItem const& source, TreeItem const& target_parent, std::size_t target_index) = 0;
        virtual void on_select(int button, TreeItem const&) = 0;
    };

    /**
     * @brief Widget that displays a Tree.
     */
    class TreeView : public Widget {
    public:
        // bool show_root_item = true;
        std::unique_ptr<TreeModel> m_model;

        TreeView(Options);
        virtual void draw() override;
        virtual bool contains(glm::vec2) const override;
        virtual glm::vec2 minimal_size() override;
        void unselect() { m_selected_item = {}; }

        void update_cache();

    protected:
        const int m_indent_size = 20;
        const int m_offset_x_button = -12;
        const int m_offset_x_left = 15;
        std::list<TreeItem> m_item_cache;
        std::map<std::size_t, bool> m_item_collapsed;
        bool m_items_changed = true;
        Rectangle* m_item_highlight_rect;
        std::optional<std::size_t> m_selected_item;

        std::unique_ptr<Triangle> m_collapse_button;
        std::unique_ptr<Text> m_text_shape;

        TreeItem* get_item_at_local_position(glm::vec2);
        TreeItem* get_item_from_cache(std::size_t hash);
        std::optional<TreeItemPosition> get_position_from_cache(std::size_t hash);
        virtual void on_update() override;
        virtual void on_click(ClickEvent&) override;
    };

    class EntityTreeModel : public TreeModel {
    public:
        std::function<void(ecs::Entity&)> select_callback;
        std::function<void(ecs::Entity&)> select_secundary_callback;

        std::shared_ptr<ecs::Entity> root_entity;

        EntityTreeModel() = default;

        virtual std::optional<TreeItem> item(TreeItem const* parent, std::size_t local_index) override;
        virtual void move_item(TreeItem const& source, TreeItem const& target_parent, std::size_t target_index) override;
        virtual void on_select(int button, TreeItem const&) override;
    };

}
