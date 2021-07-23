#include "ui/widgets/TreeView.hpp"

#include "core/Input.hpp"
#include "scene/GameObject.hpp"
#include "scene/Scene.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    TreeItem::TreeItem(std::string text, TreeView* treeview)
        : text(text)
        , m_treeview(treeview)
        , m_collapse_button(std::make_unique<Rectangle>(0_px, 10_px, "#900000")) { }

    TreeItem& TreeItem::add_child(std::string text) {
        m_treeview->m_items_changed = true;
        return children.emplace_back(text, m_treeview);
    }

    void TreeItem::set_treeview(TreeView* treeview) {
        m_treeview = treeview;
        for (auto& child : children)
            child.set_treeview(treeview);
    }

    TreeView::TreeView(UIVector pos, UIVector size, Placement placement)
        : Widget(pos, size, placement), m_root_item(TreeItem("Root", this)) { }

    void TreeView::draw() {
        int offset_y = theme->line_height;
        glm::mat4 move = normalizedMove();
        for (const auto& row : m_flat_tree_list) {
            if (!row.second.children.empty()) {
                row.second.m_collapse_button->position(UIVector(row.first * m_indent_size + m_offset_x_left + m_offset_x_button, m_actual_size.y - offset_y));
                row.second.m_collapse_button->draw(move);
            }
            theme->text_renderer()->renderText(row.second.text, m_actual_pos.x + row.first * m_indent_size + m_offset_x_left, m_actual_pos.y + m_actual_size.y - offset_y, theme->font_size, theme->color_fg);
            offset_y += theme->line_height;
        }
    }

    glm::vec2 TreeView::minimalSize() {
        float max_width = 0;
        for (const auto& row : m_flat_tree_list) {
            float width = theme->text_renderer()->textSize(row.second.text, theme->font_size).x;
            width += row.first * m_indent_size + m_offset_x_left;
            if (width > max_width)
                max_width = width;
        }
        return glm::vec2(max_width, m_flat_tree_list.size() * theme->line_height);
    }

    void TreeView::on_update() {
        Widget::on_update();
        if (m_items_changed) {
            update_flat_tree_list();
            m_items_changed = false;
        }
    }

    void TreeView::on_click(InputClickEvent* event) {
        if (event->button != GLFW_MOUSE_BUTTON_LEFT || event->action != GLFW_PRESS)
            return;

        glm::vec2 local_pos = Input::cursorPos() - m_actual_pos;
        local_pos = glm::vec2(local_pos.x, m_actual_size.y - local_pos.y);
        int offset_y = 0;
        for (auto& item : m_flat_tree_list) {
            offset_y += theme->line_height;
            if (local_pos.y < offset_y) {
                if (local_pos.x > m_offset_x_left + item.first * m_indent_size) {
                    if (callback_select)
                        callback_select(item.second);
                    return;
                }
                if (item.second.m_collapse_button->contains(Input::cursorPos() - m_actual_pos)) {
                    item.second.collapsed = !item.second.collapsed;
                    update_flat_tree_list();
                }
                return;
            }
        }
    }

    void TreeView::on_key(InputKeyEvent* event) {

    }

    void TreeView::on_char(InputCharEvent* event) {

    }

    void TreeView::on_mouse_enter() {

    }

    void TreeView::on_mouse_leave() {

    }

    void TreeView::on_focus_lost() {

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

    void TreeView::sync_scene_tree(Scene* scene) {
        std::function<void(const GameObject*, TreeItem&)> traverse = [&](const GameObject* object, TreeItem& item) {
            for (const auto& child : object->children()) {
                auto it = std::find_if(item.children.begin(), item.children.end(), [&](const TreeItem& current_item) {
                    return current_item.data.type() == typeid(GameObject*) && std::any_cast<GameObject*>(current_item.data) == child.get();
                });
                if (it == item.children.end()) {
                    std::string name = child->name;
                    if (name.empty())
                        name = "Unnamed Object";
                    TreeItem& new_item = item.add_child(name);
                    new_item.data = child.get();
                    traverse(child.get(), new_item);
                    continue;
                }
                traverse(child.get(), *it);
            }
        };
        m_root_item.text = scene->name;
        m_root_item.data = (GameObject*) scene;
        traverse(scene, m_root_item);
    }

}
