#pragma once

#include "core/Base.hpp"

namespace Birdy3d::ui {

    enum class Placement {
        TOP_LEFT,
        BOTTOM_LEFT,
        TOP_RIGHT,
        BOTTOM_RIGHT,
        CENTER_LEFT,
        CENTER_RIGHT,
        TOP_CENTER,
        BOTTOM_CENTER,
        CENTER
    };

    class Dimension {
    public:
        int pixels{0};
        float percent{0};
        float em{0};

        static Dimension make_zero() { return Dimension{0, 0.0f, 0.0f}; }
        static Dimension make_pixels(int pixels) { return Dimension{pixels, 0.0f, 0.0f}; }
        static Dimension make_percent(float percent) { return Dimension{0, percent, 0.0f}; }
        static Dimension make_em(float em) { return Dimension{0, 0.0f, em}; }

        Dimension() = default;

        bool operator==(Dimension const& other) const { return pixels == other.pixels && percent == other.percent && em == other.em; }
        bool operator<(Dimension const& other) const { return to_pixels() < other.to_pixels(); }
        bool operator<=(Dimension const& other) const { return to_pixels() <= other.to_pixels(); }
        bool operator>(Dimension const& other) const { return to_pixels() > other.to_pixels(); }
        bool operator>=(Dimension const& other) const { return to_pixels() >= other.to_pixels(); }

        Dimension operator+(Dimension const& other) const { return Dimension(pixels + other.pixels, percent + other.percent, em + other.em); }
        Dimension operator-() const { return Dimension(-pixels, -percent, -em); }
        Dimension operator-(Dimension const& other) const { return Dimension(pixels - other.pixels, percent - other.percent, em - other.em); }
        Dimension operator*(int other) const { return Dimension(pixels * other, percent * other, em * other); }
        Dimension& operator+=(Dimension const& other);
        Dimension& operator-=(Dimension const& other);

        [[nodiscard]] int to_pixels(int parent_size = 0) const;

    private:
        Dimension(int pixels, float percent, float em)
            : pixels(pixels)
            , percent(percent)
            , em(em)
        { }
    };

    class Size;

    class Position {
    public:
        Dimension x;
        Dimension y;

        static Position make_zero() { return Position{}; }
        static Position make_pixels(int x, int y) { return Position{Dimension::make_pixels(x), Dimension::make_pixels(y)}; }
        static Position make_pixels(glm::ivec2 vector) { return Position{Dimension::make_pixels(vector.x), Dimension::make_pixels(vector.y)}; }
        static Position make_percent(float x, float y) { return Position{Dimension::make_percent(x), Dimension::make_percent(y)}; }
        static Position make_percent(glm::vec2 vector) { return Position{Dimension::make_percent(vector.x), Dimension::make_percent(vector.y)}; }
        static glm::ivec2 get_relative_position(Position pos, Size size, glm::ivec2 parent_size, Placement placement);

        Position() = default;
        Position(Position const&) = default;

        Position(Dimension both)
            : x(both)
            , y(both){};

        Position(Dimension x, Dimension y)
            : x(x)
            , y(y){};

        Position& operator=(Position const& other);
        bool operator==(Position const& other) const { return x == other.x && y == other.y; }
        Position operator+(Position const& other) const { return Position(x + other.x, y + other.y); }
        Position operator-() const { return Position(-x, -y); }
        Position operator-(Position const& other) const { return Position(x - other.x, y - other.y); }
        Position& operator+=(Position const& other);
        Position& operator-=(Position const& other);

        Position operator+(Size const& other) const;
        Position operator-(Size const& other) const;
        Position& operator+=(Size const& other);
        Position& operator-=(Size const& other);

        [[nodiscard]] glm::ivec2 to_pixels(glm::ivec2 parent_size = glm::ivec2{0}) const
        {
            return glm::ivec2(x.to_pixels(parent_size.x), y.to_pixels(parent_size.y));
        }
    };

    class Size {
    public:
        Dimension x;
        Dimension y;

        static Size make_zero() { return Size{}; }
        static Size make_pixels(int x, int y) { return Size{Dimension::make_pixels(x), Dimension::make_pixels(y)}; }
        static Size make_pixels(glm::ivec2 vector) { return Size{Dimension::make_pixels(vector.x), Dimension::make_pixels(vector.y)}; }
        static Size make_percent(float x, float y) { return Size{Dimension::make_percent(x), Dimension::make_percent(y)}; }
        static Size make_percent(glm::vec2 vector) { return Size{Dimension::make_percent(vector.x), Dimension::make_percent(vector.y)}; }

        Size() = default;
        Size(Size const&) = default;

        Size(Dimension both)
            : x(both)
            , y(both){};

        Size(Dimension x, Dimension y)
            : x(x)
            , y(y){};

        Size& operator=(Size const& other);
        bool operator==(Size const& other) const { return x == other.x && y == other.y; }
        Size operator+(Size const& other) const { return Size(x + other.x, y + other.y); }
        Size operator-() const { return Size(-x, -y); }
        Size operator-(Size const& other) const { return Size(x - other.x, y - other.y); }
        Size& operator+=(Size const& other);
        Size& operator-=(Size const& other);

        [[nodiscard]] glm::ivec2 to_pixels(glm::ivec2 parent_size = glm::ivec2{0}) const
        {
            return glm::ivec2(x.to_pixels(parent_size.x), y.to_pixels(parent_size.y));
        }
    };

    inline namespace literals {

        Dimension operator"" _px(unsigned long long value);
        Dimension operator"" _pc(long double value);
        Dimension operator"" _pc(unsigned long long value);
        Dimension operator"" _em(long double value);
        Dimension operator"" _em(unsigned long long value);

    }

}
