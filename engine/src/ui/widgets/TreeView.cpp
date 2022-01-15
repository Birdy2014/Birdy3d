#include "ui/widgets/TreeView.hpp"

#include "core/Input.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Scene.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Triangle.hpp"
#include "ui/widgets/ContextMenu.hpp"

namespace Birdy3d::ui {

    TreeItem::TreeItem(std::string text, TreeView* treeview)
        : text(std::make_unique<Text>(0_px, text, utils::Color::Name::FG, Placement::TOP_LEFT))
        , m_treeview(treeview)
        , m_collapse_button(std::make_unique<Triangle>(0_px, UIVector(core::Application::theme().font_size() / 2), utils::Color::Name::FG)) { }

    TreeItem& TreeItem::add_child(std::string text) {
        m_treeview->m_items_changed = true;
        return children.emplace_back(text, m_treeview);
    }

    void TreeItem::set_treeview(TreeView* treeview) {
        m_treeview = treeview;
        for (auto& child : children)
            child.set_treeview(treeview);
    }

    void TreeItem::remove_child(const TreeItem* child) {
        children.remove_if([&](TreeItem& item) { return &item == child; });
    }

    TreeView::TreeView(Options options)
        : Widget(options)
        , m_root_item(TreeItem("Root", this)) {
        m_item_highlight_rect = add_filled_rectangle(0_px, UIVector(100_p, core::Application::theme().line_height()), utils::Color::Name::BG_SELECTED, Placement::TOP_LEFT);
        m_item_highlight_rect->hidden(true);
    }

    void TreeView::draw() {
        int offset_y = 0;
        for (const auto& row : m_flat_tree_list) {
            if (m_selected_item == &row.second) {
                m_item_highlight_rect->position(UIVector(0_px, offset_y));
                m_item_highlight_rect->draw(m_move);
            }
            if (!row.second.children.empty()) {
                row.second.m_collapse_button->position(glm::vec2(row.first * m_indent_size + m_offset_x_left + m_offset_x_button, offset_y + (core::Application::theme().line_height() - row.second.m_collapse_button->size().x) / 2.0f));
                row.second.m_collapse_button->rotation(glm::radians(row.second.collapsed ? 30.0f : 60.0f));
                row.second.m_collapse_button->draw(m_move);
            }
            row.second.text->position(UIVector(row.first * m_indent_size + m_offset_x_left, offset_y));
            row.second.text->draw(m_move);
            offset_y += core::Application::theme().line_height();
        }
    }

    glm::vec2 TreeView::minimal_size() {
        float max_width = 0;
        for (const auto& row : m_flat_tree_list) {
            float width = row.second.text->size().x;
            width += row.first * m_indent_size + m_offset_x_left;
            if (width > max_width)
                max_width = width;
        }
        return glm::vec2(max_width, m_flat_tree_list.size() * core::Application::theme().line_height());
    }

    void TreeView::on_update() {
        Widget::on_update();
        if (m_items_changed) {
            update_flat_tree_list();
            m_items_changed = false;
        }
    }

    void TreeView::on_click(const events::InputClickEvent& event) {
        if (event.action != GLFW_PRESS)
            return;

        glm::vec2 local_pos = core::Input::cursor_pos() - m_actual_pos;
        int offset_y = 0;
        for (auto& item : m_flat_tree_list) {
            offset_y += core::Application::theme().line_height();
            if (local_pos.y < offset_y) {
                if (local_pos.x > m_offset_x_left + item.first * m_indent_size) {
                    // Select
                    m_item_highlight_rect->hidden(false);
                    m_selected_item = &item.second;
                    execute_callbacks("select", &item.second);
                    if (event.button == GLFW_MOUSE_BUTTON_RIGHT)
                        execute_callbacks("select_secundary", &item.second);
                } else if (local_pos.x > m_offset_x_left + (item.first - 1) * m_indent_size) {
                    // Toggle children
                    item.second.collapsed = !item.second.collapsed;
                    update_flat_tree_list();
                }
                return;
            }
        }
    }

    void TreeView::update_flat_tree_list() {
        m_flat_tree_list.clear();
        update_flat_tree_list(m_root_item, 0);
    }

    void TreeView::update_flat_tree_list(TreeItem& item, int indent) {
        m_flat_tree_list.push_back(std::pair<int, TreeItem&>(indent, item));
        if (item.collapsed)
            return;
        for (TreeItem& child : item.children) {
            update_flat_tree_list(child, indent + 1);
        }
    }

    void TreeView::sync_scene_tree(ecs::Scene* scene) {
        std::function<void(const ecs::Entity*, TreeItem&)> traverse = [&](const ecs::Entity* entity, TreeItem& item) {
            // Remove old TreeItems
            for (auto child_item_it = item.children.cbegin(); child_item_it != item.children.cend();) {
                const auto& child_item = *child_item_it;
                child_item_it++;
                if (child_item.data.type() != typeid(ecs::Entity*)) {
                    item.remove_child(&child_item);
                    continue;
                }
                auto it = std::find_if(entity->children().begin(), entity->children().end(), [&](const std::shared_ptr<ecs::Entity>& child_entity) {
                    return child_entity.get() == std::any_cast<ecs::Entity*>(child_item.data);
                });
                if (it == entity->children().end()) {
                    item.remove_child(&child_item);
                }
            }

            // Add new TreeItems
            for (const auto& child : entity->children()) {
                auto it = std::find_if(item.children.begin(), item.children.end(), [&](const TreeItem& child_item) {
                    return child_item.data.type() == typeid(ecs::Entity*) && std::any_cast<ecs::Entity*>(child_item.data) == child.get();
                });
                if (it == item.children.end()) {
                    std::string name = child->name;
                    if (name.empty())
                        name = "Unnamed Entity";
                    TreeItem& new_item = item.add_child(name);
                    new_item.data = child.get();
                    traverse(child.get(), new_item);
                    continue;
                }
                traverse(child.get(), *it);
            }
        };
        m_root_item.text->text(scene->name);
        m_root_item.data = (ecs::Entity*)scene;
        traverse(scene, m_root_item);
        update_flat_tree_list();
    }

}
