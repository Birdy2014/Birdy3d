#include "ui/widgets/TreeView.hpp"

#include "core/Input.hpp"
#include "ecs/Entity.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include <stack>

namespace Birdy3d::ui {

    TreeView::TreeView(Options options)
        : Widget(options)
    { }

    void TreeView::draw()
    {
        int offset_y = 0;
        for (auto const& item : m_item_cache) {
            if (m_selected_item.has_value() && m_selected_item.value() == item.hash) {
                auto line_height = core::Application::theme().line_height();
                auto bg_selected_color = core::Application::theme().color(utils::Color::Name::BG_SELECTED);
                auto position = glm::ivec2(0, offset_y);
                auto size = glm::ivec2(m_absolute_rect.width(), line_height);
                paint_rectangle_filled(Rect::from_position_and_size(position, size), bg_selected_color);
            }
            if (!item.is_leaf) {
                auto line_height = core::Application::theme().line_height();
                auto fg_color = core::Application::theme().color(utils::Color::Name::FG);
                auto size = glm::ivec2(line_height / 2.0f);
                auto position = glm::ivec2(item.depth * m_indent_size + m_offset_x_left + m_offset_x_button, offset_y);
                auto orientation = glm::radians(m_item_collapsed[item.hash] ? 30.0f : 0.0f);
                paint_triangle_filled(Rect::from_position_and_size(position, size), orientation, fg_color);
            }
            auto text = TextDescription(item.text);
            paint_text(glm::ivec2(item.depth * m_indent_size + m_offset_x_left, offset_y), text);
            offset_y += core::Application::theme().line_height();
        }
    }

    bool TreeView::contains(glm::ivec2 point) const
    {
        return m_visible_area.contains(point);
    }

    glm::ivec2 TreeView::minimal_size()
    {
        float max_width = 0;
        for (auto const& item : m_item_cache) {
            auto width = core::Application::theme().text_renderer().text_size(item.text, core::Application::theme().font_size()).x.to_pixels();
            width += item.depth * m_indent_size + m_offset_x_left;
            if (width > max_width)
                max_width = width;
        }
        return glm::ivec2(max_width, m_item_cache.size() * core::Application::theme().line_height());
    }

    void TreeView::on_update()
    {
        Widget::on_update();
        if (m_items_changed) {
            update_cache();
            m_items_changed = false;
        }
    }

    TreeItem* TreeView::get_item_at_local_position(glm::ivec2 local_pos)
    {
        int offset_y = 0;
        for (auto& item : m_item_cache) {
            offset_y += core::Application::theme().line_height();
            if (local_pos.y < offset_y)
                return &item;
        }
        return {};
    }

    TreeItem* TreeView::get_item_from_cache(std::size_t hash)
    {
        for (auto& item : m_item_cache) {
            if (item.hash == hash)
                return &item;
        }
        return nullptr;
    }

    std::optional<TreeItemPosition> TreeView::get_position_from_cache(std::size_t hash)
    {
        TreeItem* found_item = nullptr;
        std::size_t depth;
        for (auto it = std::rbegin(m_item_cache); it != std::rend(m_item_cache); ++it) {
            if (it->hash == hash) {
                found_item = &*it;
                depth = it->depth;
            }

            if (found_item && it->depth < depth) {
                return TreeItemPosition{
                    .item = *found_item,
                    .parent = *it,
                    .local_index = found_item->local_index};
            }
        }

        return {};
    }

    void TreeView::on_click(ClickEvent& event)
    {
        auto local_pos = core::Input::cursor_pos_int() - m_absolute_rect.position();

        if (event.action == GLFW_RELEASE) {
            ungrab_cursor();

            // Handle move
            auto item = get_item_at_local_position(local_pos);
            if (!item || !m_selected_item.has_value())
                return;

            auto source_item = get_item_from_cache(m_selected_item.value());

            if (!source_item || item == source_item)
                return;

            auto target_position = get_position_from_cache(item->hash);
            if (!target_position.has_value())
                return;

            m_model->move_item(*source_item, target_position->parent, target_position->local_index);

            update_cache();
            return;
        }

        if (event.action != GLFW_PRESS)
            return;

        event.handled();

        // FIXME: Grab cursor and change cursor shape on cursor move if key is pressed down
        grab_cursor();

        auto item = get_item_at_local_position(local_pos);
        if (!item)
            return;

        if (local_pos.x > m_offset_x_left + static_cast<int>(item->depth) * m_indent_size) {
            // Select
            m_selected_item = item->hash;
            m_model->on_select(event.button, *item);
        } else if (local_pos.x > m_offset_x_left + (static_cast<int>(item->depth) - 1) * m_indent_size) {
            // Toggle children
            m_item_collapsed[item->hash] = !m_item_collapsed[item->hash];
            update_cache();
        }
    }

    void TreeView::update_cache()
    {
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

    std::optional<TreeItem> EntityTreeModel::item(TreeItem const* parent, std::size_t local_index)
    {
        if (!root_entity)
            return {};

        auto construct_item = [local_index](ecs::Entity* entity) {
            return TreeItem{
                .text = entity->name.empty() ? "Unnamed Entity" : entity->name,
                .data = std::make_any<ecs::Entity*>(entity),
                .local_index = local_index,
                .is_leaf = entity->children().empty(),
                .hash = reinterpret_cast<std::size_t>(entity)};
        };

        ecs::Entity* parent_entity{};
        ecs::Entity* found_entity{};

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

    void EntityTreeModel::move_item(TreeItem const& source, TreeItem const& target_parent, std::size_t target_index)
    {
        auto source_entity = std::any_cast<ecs::Entity*>(source.data);
        auto target_parent_entity = std::any_cast<ecs::Entity*>(target_parent.data);

        if (target_parent_entity->is_descendant_of(*source_entity) || !source_entity->parent)
            return;

        auto moved_child = source_entity->parent->move_child_out(source_entity);
        if (!moved_child)
            return;

        target_parent_entity->add_child_at(target_index, moved_child);
    }

    void EntityTreeModel::on_select(int button, TreeItem const& item)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && select_callback)
            std::invoke(select_callback, *std::any_cast<ecs::Entity*>(item.data));
        if (button == GLFW_MOUSE_BUTTON_RIGHT && select_secundary_callback)
            std::invoke(select_secundary_callback, *std::any_cast<ecs::Entity*>(item.data));
    }

}
