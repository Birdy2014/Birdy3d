#pragma once

#include "ui/Units.hpp"
#include <glm/glm.hpp>

namespace Birdy3d::ui {

    class Rect {
    public:
        Rect() = default;
        ~Rect() = default;

        static Rect from_position_and_size(glm::ivec2 position, glm::ivec2 size)
        {
            if (size.x < 0)
                size.x = 0;
            if (size.y < 0)
                size.y = 0;
            return Rect{position, size};
        }

        static Rect from_top_left_and_bottom_right(glm::ivec2 top_left, glm::ivec2 bottom_right)
        {
            if (bottom_right.x < top_left.x)
                bottom_right.x = top_left.x;
            if (bottom_right.y < top_left.y)
                bottom_right.y = top_left.y;
            return Rect{top_left, bottom_right - top_left};
        }

        [[nodiscard]] glm::ivec2 position() const
        {
            return m_position;
        }

        bool position(glm::ivec2 position)
        {
            m_position = position;
            return true;
        }

        [[nodiscard]] glm::ivec2 size() const
        {
            return m_size;
        }

        bool size(glm::ivec2 size)
        {
            if (size.x < 0 || size.y < 0) {
                m_size.x = std::max(size.x, 0);
                m_size.y = std::max(size.y, 0);
                return false;
            }
            m_size = size;
            return true;
        }

        [[nodiscard]] glm::ivec2 top_left() const
        {
            return m_position;
        }

        bool top_left(glm::ivec2 top_left)
        {
            auto bottom_right = m_position + m_size;
            m_position = top_left;
            m_size = bottom_right - top_left;

            if (m_size.x < 0 || m_size.y < 0) {
                m_size.x = std::max(m_size.x, 0);
                m_size.y = std::max(m_size.y, 0);
                return false;
            }
            return true;
        }

        [[nodiscard]] glm::ivec2 bottom_right() const
        {
            return m_position + m_size;
        }

        bool bottom_right(glm::ivec2 bottom_right)
        {
            return size(bottom_right - top_left());
        }

        [[nodiscard]] int left() const
        {
            return m_position.x;
        }

        bool left(int x)
        {
            m_position.x = x;
            return true;
        }

        [[nodiscard]] int top() const
        {
            return m_position.y;
        }

        bool top(int y)
        {
            m_position.y = y;
            return true;
        }

        [[nodiscard]] int width() const
        {
            return size().x;
        }

        bool width(int x)
        {
            if (x < 0) {
                m_size.x = 0;
                return false;
            }
            m_size.x = x;
            return true;
        }

        [[nodiscard]] int height() const
        {
            return size().y;
        }

        bool height(int y)
        {
            if (y < 0) {
                m_size.y = 0;
                return false;
            }
            m_size.y = y;
            return true;
        }

        [[nodiscard]] bool contains(glm::ivec2 point) const
        {
            return point.x > top_left().x && point.y > top_left().y && point.x < bottom_right().x && point.y < bottom_right().y;
        }

        void shrink_to(Rect const& other)
        {
            auto my_top_left = top_left();
            auto my_bottom_right = bottom_right();
            auto other_top_left = other.top_left();
            auto other_bottom_right = other.bottom_right();

            top_left(glm::ivec2(std::max(my_top_left.x, other_top_left.x), std::max(my_top_left.y, other_top_left.y)));
            bottom_right(glm::ivec2(std::min(my_bottom_right.x, other_bottom_right.x), std::min(my_bottom_right.y, other_bottom_right.y)));
        }

    private:
        Rect(glm::ivec2 position, glm::ivec2 size)
            : m_position{position}
            , m_size{size}
        { }

        glm::ivec2 m_position{};
        glm::ivec2 m_size{};
    };

    class DimRect {
    public:
        Placement placement;

        DimRect() = default;
        ~DimRect() = default;

        static DimRect from_position_and_size(Position position, Size size, Placement placement)
        {
            // FIXME: Catch negative size
            return DimRect{position, size, placement};
        }

        static DimRect from_top_left_and_bottom_right(Position top_left, Position bottom_right, Placement placement)
        {
            // FIXME: Catch negative size
            return DimRect{top_left, Size::from_position(bottom_right - top_left), placement};
        }

        [[nodiscard]] Position position() const
        {
            return m_position;
        }

        bool position(Position position)
        {
            m_position = position;
            return true;
        }

        [[nodiscard]] Size size() const
        {
            return m_size;
        }

        bool size(Size size)
        {
            if (size.x.to_pixels(1) > 0 && size.y.to_pixels(1) < 0) {
                m_size.x = size.x;
                return false;
            }
            if (size.x.to_pixels(1) < 0 && size.y.to_pixels(1) > 0) {
                m_size.y = size.y;
                return false;
            }
            m_size = size;
            return true;
        }

        [[nodiscard]] Position top_left() const
        {
            return m_position;
        }

        [[nodiscard]] Position bottom_right() const
        {
            return m_position + m_size;
        }

        [[nodiscard]] Dimension left() const
        {
            return position().x;
        }

        bool left(Dimension x)
        {
            return position(Position{x, position().y});
        }

        [[nodiscard]] Dimension top() const
        {
            return position().y;
        }

        bool top(Dimension y)
        {
            return position(Position{position().x, y});
        }

        [[nodiscard]] Dimension width() const
        {
            return size().x;
        }

        bool width(Dimension x)
        {
            return size(Size{x, size().y});
        }

        [[nodiscard]] Dimension height() const
        {
            return size().y;
        }

        bool height(Dimension y)
        {
            return size(Size{size().x, y});
        }

        [[nodiscard]] Rect to_rect(Rect const& parent) const
        {
            auto relative_position = Position::get_relative_position(m_position, m_size, parent.size(), placement);
            auto absolute_position = parent.position() + relative_position;
            auto size = m_size.to_pixels(parent.size());
            return Rect::from_position_and_size(absolute_position, size);
        }

        [[nodiscard]] Rect to_rect() const
        {
            return Rect::from_position_and_size(position().to_pixels(), size().to_pixels());
        }

        [[nodiscard]] bool contains(glm::ivec2 point, Rect const& parent) const
        {
            auto rect = to_rect(parent);
            return rect.contains(point);
        }

        void operator+=(Size size_diff)
        {
            size(m_size + size_diff);
        }

        void operator-=(Size size_diff)
        {
            size(m_size - size_diff);
        }

    private:
        DimRect(Position position, Size size, Placement placement)
            : placement{placement}
            , m_position{position}
            , m_size{size}
        { }

        Position m_position{};
        Size m_size{};
    };

}

template <>
struct fmt::formatter<Birdy3d::ui::Rect> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}')
            throw format_error("invalid format");
        return it;
    }

    template <typename FormatContext>
    auto format(Birdy3d::ui::Rect const& rect, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "Rect{{left: {}, top: {}, width: {}, height: {}}}", rect.top_left().x, rect.top_left().y, rect.width(), rect.height());
    }
};
