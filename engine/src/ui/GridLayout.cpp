#include "ui/GridLayout.hpp"

#include "ui/Widget.hpp"
#include <glm/gtx/string_cast.hpp>
#include <map>
#include <numeric>

namespace Birdy3d::ui {

    StaticGridLayout::StaticGridLayout(int gap)
        : m_gap(gap)
    { }

    void StaticGridLayout::arrange(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const
    {
        // Minimal widths/heights
        std::map<int, int> min_widths; // Widths of columns
        std::map<int, int> min_heights; // Height of rows
        std::map<int, int> max_widths; // Widths of columns
        std::map<int, int> max_heights; // Height of rows
        for (auto const& child : children) {
            auto minimal = child->minimal_size();
            if (minimal.x > min_widths[child->column])
                min_widths[child->column] = minimal.x;
            if (minimal.y > min_heights[child->row])
                min_heights[child->row] = minimal.y;

            auto preferred = child->preferred_size(size);
            if (preferred.x > max_widths[child->column])
                max_widths[child->column] = preferred.x;
            if (preferred.y > max_heights[child->row])
                max_heights[child->row] = preferred.y;
        }

        // expand widths/heights to fit the size argument
        int min_width_sum = std::accumulate(min_widths.begin(), min_widths.end(), 0, [](int sum, std::pair<int, int> p) { return sum + p.second; });
        int min_height_sum = std::accumulate(min_heights.begin(), min_heights.end(), 0, [](int sum, std::pair<int, int> p) { return sum + p.second; });

        int max_width_sum = std::accumulate(max_widths.begin(), max_widths.end(), 0, [](int sum, std::pair<int, int> p) { return sum + p.second; });
        int max_height_sum = std::accumulate(max_heights.begin(), max_heights.end(), 0, [](int sum, std::pair<int, int> p) { return sum + p.second; });
        int gapless_width = size.x - m_gap * std::max((int)min_widths.size() - 1, 0);
        int gapless_height = size.y - m_gap * std::max((int)min_heights.size() - 1, 0);
        int target_width = std::min(max_width_sum, gapless_width);
        int target_height = std::min(max_height_sum, gapless_height);

        std::vector<int> smaller_columns;
        std::vector<int> smaller_rows;
        for ([[maybe_unused]] auto& [column, width] : min_widths)
            smaller_columns.push_back(column);
        for ([[maybe_unused]] auto& [row, height] : min_heights)
            smaller_rows.push_back(row);
        std::map<int, int> result_widths = min_widths;
        std::map<int, int> result_heights = min_heights;
        int result_width_sum = min_width_sum;
        int result_height_sum = min_height_sum;

        while (result_width_sum + 1 < target_width || result_height_sum + 1 < target_height) {
            for (auto it = smaller_columns.begin(); it != smaller_columns.end();) {
                int inc_amount = std::min((target_width - result_width_sum) / (int)smaller_columns.size(), max_widths[*it] - result_widths[*it]);
                result_width_sum += inc_amount;
                result_widths[*it] += inc_amount;
                if (inc_amount == max_widths[*it]) {
                    it = smaller_columns.erase(it);
                    continue;
                }
                ++it;
            }
            for (auto it = smaller_rows.begin(); it != smaller_rows.end();) {
                int inc_amount = std::min((target_height - result_height_sum) / (int)smaller_rows.size(), max_heights[*it]);
                result_height_sum += inc_amount;
                result_heights[*it] += inc_amount;
                if (inc_amount == max_heights[*it]) {
                    it = smaller_rows.erase(it);
                    continue;
                }
                ++it;
            }
        }

        // Set position and size of widgets
        for (auto const& child : children) {
            glm::ivec2 widget_position = pos;
            for (auto const& [column, width] : result_widths) {
                if (column >= child->column)
                    break;
                widget_position.x += width + m_gap;
            }
            for (auto const& [row, height] : result_heights) {
                if (row >= child->row)
                    break;
                widget_position.y += height + m_gap;
            }

            glm::ivec2 size;
            size.x = result_widths[child->column];
            size.y = result_heights[child->row];

            child->arrange(widget_position, size);
        }
    }

    glm::ivec2 StaticGridLayout::minimal_size(std::list<std::shared_ptr<Widget>> const& children) const
    {
        std::map<int, int> widths;
        std::map<int, int> heights;
        for (auto const& child : children) {
            if (child->minimal_size().x > widths[child->column])
                widths[child->column] = child->minimal_size().x;
            if (child->minimal_size().y > heights[child->row])
                heights[child->row] = child->minimal_size().y;
        }

        glm::ivec2 size{0};
        for ([[maybe_unused]] auto const& [column, width] : widths)
            size.x += width + m_gap;
        size.x -= m_gap;
        if (size.x < 0)
            size.x = 0;

        for ([[maybe_unused]] auto const& [row, height] : heights)
            size.y += height + m_gap;
        size.y -= m_gap;
        if (size.y < 0)
            size.y = 0;

        return size;
    }

    DynamicGridLayout::DynamicGridLayout(int gap)
        : m_gap(gap)
    { }

    void DynamicGridLayout::arrange(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const
    {
        glm::ivec2 offset{0};
        float row_height = 0;
        for (auto const& child : children) {
            auto child_size = child->minimal_size();
            if (offset.x + child_size.x > size.x) {
                offset.y += row_height + m_gap;
                row_height = 0;
                offset.x = 0;
            }
            child->arrange(pos + offset, child_size);
            offset.x += child_size.x + m_gap;
            if (row_height < child_size.y)
                row_height = child_size.y;
        }
    }

    glm::ivec2 DynamicGridLayout::minimal_size([[maybe_unused]] std::list<std::shared_ptr<Widget>> const& children) const
    {
        // TODO: DynamicGridLayout minimal_size
        return {};
    }

    glm::ivec2 DynamicGridLayout::minimal_size(std::list<std::shared_ptr<Widget>> const& children, float suggested_size, Layout::Direction direction) const
    {
        if (direction == Layout::Direction::HORIZONTAL) {
            glm::ivec2 offset{0};
            float row_height = 0;
            for (auto const& child : children) {
                auto child_size = child->minimal_size();
                if (offset.x + child_size.x > suggested_size) {
                    offset.y += row_height + m_gap;
                    row_height = 0;
                    offset.x = 0;
                }
                offset.x += child_size.x + m_gap;
                if (row_height < child_size.y)
                    row_height = child_size.y;
            }
            offset.y += row_height;
            return offset;
        } else {
            // TODO: Vertical
            BIRDY3D_TODO
        }
        return {};
    }

    float DynamicGridLayout::minimal_size(std::list<std::shared_ptr<Widget>> const& children, Layout::Direction direction) const
    {
        float minsize = 0;
        for (auto const& child : children) {
            auto direction_size = direction == Layout::Direction::HORIZONTAL ? child->minimal_size().x : child->minimal_size().y;
            if (direction_size > minsize)
                minsize = direction_size;
        }
        return minsize;
    }

}
