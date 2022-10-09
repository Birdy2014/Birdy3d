#include "ui/widgets/TreeView.hpp"

#include "core/Input.hpp"
#include "ecs/Entity.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Triangle.hpp"
#include <stack>

namespace Birdy3d::ui {

    TreeView::TreeView(Options options)
        : Widget(options)
        , m_collapse_button(std::make_unique<Triangle>(0_px, UIVector(core::Application::theme().font_size() / 2), utils::Color::Name::FG))
        , m_text_shape(std::make_unique<Text>(0_px, "", utils::Color::Name::FG, Placement::TOP_LEFT)) {
        m_item_highlight_rect = add_filled_rectangle(0_px, UIVector(100_p, core::Application::theme().line_height()), utils::Color::Name::BG_SELECTED, Placement::TOP_LEFT);
        m_item_highlight_rect->hidden(true);
    }

    void TreeView::draw() {
        int offset_y = 0;
        for (const auto& item : m_item_cache) {
            if (m_selected_item.has_value() && m_selected_item.value() == item.hash) {
                m_item_highlight_rect->position(UIVector(0_px, offset_y));
                m_item_highlight_rect->draw(m_move);
            }
            if (!item.is_leaf) {
                m_collapse_button->position(glm::vec2(item.depth * m_indent_size + m_offset_x_left + m_offset_x_button, offset_y + (core::Application::theme().line_height() - m_collapse_button->size().x) / 2.0f));
                m_collapse_button->rotation(glm::radians(m_item_collapsed[item.hash] ? 30.0f : 0.0f));
                m_collapse_button->draw(m_move);
            }
            m_text_shape->position(UIVector(item.depth * m_indent_size + m_offset_x_left, offset_y));
            m_text_shape->text(item.text);
            m_text_shape->draw(m_move);
            offset_y += core::Application::theme().line_height();
        }
    }

    bool TreeView::contains(glm::vec2 point) const {
        return point.x > m_visible_pos.x && point.y > m_visible_pos.y && point.x < m_visible_pos.x + m_visible_size.x && point.y < m_visible_pos.y + m_visible_size.y;
    }

    glm::vec2 TreeView::minimal_size() {
        float max_width = 0;
        for (auto const& item : m_item_cache) {
            auto width = core::Application::theme().text_renderer().text_size(item.text, core::Application::theme().font_size()).x;
            width += item.depth * m_indent_size + m_offset_x_left;
            if (width > max_width)
                max_width = width;
        }
        return glm::vec2(max_width, m_item_cache.size() * core::Application::theme().line_height());
    }

    void TreeView::on_update() {
        Widget::on_update();
        if (m_items_changed) {
            update_cache();
            m_items_changed = false;
        }
    }

    void TreeView::on_click(ClickEvent& event) {
        if (event.action != GLFW_PRESS)
            return;

        event.handled();

        glm::vec2 local_pos = core::Input::cursor_pos() - m_actual_pos;
        int offset_y = 0;
        for (auto& item : m_item_cache) {
            offset_y += core::Application::theme().line_height();
            if (local_pos.y < offset_y) {
                if (local_pos.x > m_offset_x_left + item.depth * m_indent_size) {
                    // Select
                    m_item_highlight_rect->hidden(false);
                    m_selected_item = item.hash;
                    m_model->on_select(event.button, item);
                } else if (local_pos.x > m_offset_x_left + (static_cast<int>(item.depth) - 1) * m_indent_size) {
                    // Toggle children
                    m_item_collapsed[item.hash] = !m_item_collapsed[item.hash];
                    update_cache();
                }
                return;
            }
        }
    }

    void TreeView::update_cache() {
        m_item_cache.clear();

        std::stack<TreeItem*> stack;
        bool is_forward = true;

        auto new_item = m_model->item(nullptr, 0);
        if (new_item.has_value()) {
            new_item->depth = 0;
            m_item_cache.push_back(new_item.value());
            stack.push(&m_item_cache.back());
        }

        while (!stack.empty()) {
            if (!stack.top()->is_leaf && is_forward && !m_item_collapsed[stack.top()->hash]) {
                auto child_item = m_model->item(stack.top(), 0);
                if (child_item.has_value()) {
                    child_item->depth = stack.size();
                    m_item_cache.push_back(child_item.value());
                    stack.push(&m_item_cache.back());
                    continue;
                }
            }

            auto current_index = stack.top()->local_index;
            stack.pop();
            is_forward = false;

            auto sibling_item = m_model->item(stack.empty() ? nullptr : stack.top(), current_index + 1);
            if (sibling_item.has_value()) {
                sibling_item->depth = stack.size();
                m_item_cache.push_back(sibling_item.value());
                stack.push(&m_item_cache.back());
                is_forward = true;
            }
        }
    }

    std::optional<TreeItem> EntityTreeModel::item(TreeItem const* parent, std::size_t local_index) {
        if (!root_entity)
            return {};

        auto construct_item = [local_index](ecs::Entity* entity) {
            return TreeItem {
                .text = entity->name.empty() ? "Unnamed Entity" : entity->name,
                .data = std::make_any<ecs::Entity*>(entity),
                .local_index = local_index,
                .is_leaf = entity->children().empty(),
                .hash = reinterpret_cast<std::size_t>(entity)
            };
        };

        ecs::Entity* parent_entity {};
        ecs::Entity* found_entity {};

        if (!parent) {
            if (local_index != 0)
                return {};
            return construct_item(root_entity.get());
        }

        if (parent)
            parent_entity = std::any_cast<ecs::Entity*>(parent->data);
        else
            parent_entity = root_entity.get();

        if (parent_entity && parent_entity->children().size() > local_index)
            found_entity = parent_entity->children()[local_index].get();

        if (!found_entity)
            return {};

        return construct_item(found_entity);
    }

    void EntityTreeModel::move_item(TreeItem const& source, TreeItem const& target_parent, std::size_t target_index) {
        // TODO: Implement move_item for EntityTreeModel
        assert(false);
    }

    void EntityTreeModel::on_select(int button, TreeItem const& item) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && select_callback)
            std::invoke(select_callback, *std::any_cast<ecs::Entity*>(item.data));
        if (button == GLFW_MOUSE_BUTTON_RIGHT && select_secundary_callback)
            std::invoke(select_secundary_callback, *std::any_cast<ecs::Entity*>(item.data));
    }

}
