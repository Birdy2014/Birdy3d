#include "ui/Units.hpp"

#include "core/Application.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d::ui {

    // Dimension

    Dimension& Dimension::operator+=(Dimension const& other)
    {
        pixels += other.pixels;
        percent += other.percent;
        em += other.em;
        return *this;
    }

    Dimension& Dimension::operator-=(Dimension const& other)
    {
        pixels -= other.pixels;
        percent -= other.percent;
        em -= other.em;
        return *this;
    }

    int Dimension::to_pixels(int parent_size) const
    {
        return pixels + (percent / 100) * parent_size + em * core::Application::theme().line_height();
    }

    // Position

    Position& Position::operator=(Position const& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    Position& Position::operator+=(Position const& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Position& Position::operator-=(Position const& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Position Position::operator+(Size const& other) const
    {
        return Position(x + other.x, y + other.y);
    }

    Position Position::operator-(Size const& other) const
    {
        return Position(x - other.x, y - other.y);
    }

    Position& Position::operator+=(Size const& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Position& Position::operator-=(Size const& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    glm::ivec2 Position::get_relative_position(Position pos, Size size, glm::ivec2 parent_size, Placement placement)
    {
        glm::ivec2 p = pos.to_pixels(parent_size);
        glm::ivec2 s = size.to_pixels(parent_size);
        glm::ivec2 out;

        if (placement == Placement::TOP_LEFT || placement == Placement::BOTTOM_LEFT || placement == Placement::CENTER_LEFT) {
            out.x = p.x;
        } else if (placement == Placement::TOP_RIGHT || placement == Placement::BOTTOM_RIGHT || placement == Placement::CENTER_RIGHT) {
            out.x = parent_size.x + p.x - s.x;
        } else if (placement == Placement::TOP_CENTER || placement == Placement::BOTTOM_CENTER || placement == Placement::CENTER) {
            out.x = parent_size.x / 2 - s.x / 2 + p.x;
        }
        if (placement == Placement::TOP_LEFT || placement == Placement::TOP_RIGHT || placement == Placement::TOP_CENTER) {
            out.y = p.y;
        } else if (placement == Placement::BOTTOM_LEFT || placement == Placement::BOTTOM_RIGHT || placement == Placement::BOTTOM_CENTER) {
            out.y = parent_size.y + p.y - s.y;
        } else if (placement == Placement::CENTER_LEFT || placement == Placement::CENTER_RIGHT || placement == Placement::CENTER) {
            out.y = parent_size.y / 2 - s.y / 2 + p.y;
        }
        return out;
    }

    // Size

    Size& Size::operator=(Size const& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    Size& Size::operator+=(Size const& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Size& Size::operator-=(Size const& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    // literals

    inline namespace literals {

        Dimension operator"" _px(unsigned long long value)
        {
            return Dimension::make_pixels(value);
        }

        Dimension operator"" _pc(long double value)
        {
            return Dimension::make_percent(value);
        }

        Dimension operator"" _pc(unsigned long long value)
        {
            return Dimension::make_percent(value);
        }

        Dimension operator"" _em(long double value)
        {
            return Dimension::make_em(value);
        }

        Dimension operator"" _em(unsigned long long value)
        {
            return Dimension::make_em(value);
        }

    }

}
