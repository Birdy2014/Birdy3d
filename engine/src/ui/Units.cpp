#include "ui/Units.hpp"

namespace Birdy3d::ui {

    Unit::Unit(float pixels, float percent)
        : pixels(pixels)
        , percent(percent) { }

    void Unit::operator=(float value) {
        this->pixels = value;
    }

    Unit::operator float() {
        return pixels;
    }

    float Unit::to_pixels(float parentSize) {
        return (percent / 100) * parentSize + pixels;
    }

    bool Unit::operator==(const Unit& other) const {
        return pixels == other.pixels && percent == other.percent;
    }

    Unit Unit::operator+(const Unit& other) {
        return Unit(pixels + other.pixels, percent + other.percent);
    }

    Unit& Unit::operator+=(const Unit& other) {
        pixels += other.pixels;
        percent += other.percent;
        return *this;
    }

    Unit Unit::operator-() {
        return Unit(-pixels, -percent);
    }

    Unit Unit::operator-(const Unit& other) {
        return Unit(pixels - other.pixels, percent - other.percent);
    }

    Unit& Unit::operator-=(const Unit& other) {
        pixels -= other.pixels;
        percent -= other.percent;
        return *this;
    }

    Unit operator"" _px(long double value) {
        return Unit(value, 0);
    }

    Unit operator"" _px(unsigned long long value) {
        return Unit(value, 0);
    }

    Unit operator"" _p(long double value) {
        return Unit(0, value);
    }

    Unit operator"" _p(unsigned long long value) {
        return Unit(0, value);
    }

    UIVector::UIVector()
        : x(0, 0)
        , y(0, 0) { }

    UIVector::UIVector(const UIVector& v)
        : x(v.x)
        , y(v.y) { }

    UIVector::UIVector(const glm::vec2& v)
        : x(v.x, 0)
        , y(v.y, 0) { }

    UIVector::UIVector(Unit x)
        : x(x)
        , y(x) { }

    UIVector::UIVector(Unit x, Unit y)
        : x(x)
        , y(y) { }

    UIVector& UIVector::operator=(const UIVector& other) {
        x = other.x;
        y = other.y;
        return *this;
    }

    bool UIVector::operator==(const UIVector& other) const {
        return x == other.x && y == other.y;
    }

    UIVector UIVector::operator+(const UIVector& other) {
        return UIVector(x + other.x, y + other.y);
    }

    UIVector UIVector::operator+(const float other) {
        return UIVector(x + other, y + other);
    }

    UIVector UIVector::operator-() {
        return UIVector(-x, -y);
    }

    UIVector UIVector::operator-(const UIVector& other) {
        return UIVector(x - other.x, y - other.y);
    }

    UIVector UIVector::operator+=(const UIVector& other) {
        x += other.x;
        y += other.y;
        return UIVector(x, y);
    }

    UIVector UIVector::operator+=(const float other) {
        x += other;
        y += other;
        return UIVector(x, y);
    }

    UIVector UIVector::operator-=(const UIVector& other) {
        x -= other.x;
        y -= other.y;
        return UIVector(x, y);
    }

    glm::vec2 UIVector::to_pixels(glm::vec2 parentSize) {
        return glm::vec2(x.to_pixels(parentSize.x), y.to_pixels(parentSize.y));
    }

    UIVector::operator glm::vec2() {
        return glm::vec2(x, y);
    }

    glm::vec2 UIVector::get_relative_position(UIVector pos, UIVector size, glm::vec2 parentSize, Placement placement) {
        glm::vec2 p = pos.to_pixels(parentSize);
        glm::vec2 s = size.to_pixels(parentSize);
        glm::vec2 out;

        if (placement == Placement::TOP_LEFT || placement == Placement::BOTTOM_LEFT || placement == Placement::CENTER_LEFT) {
            out.x = p.x;
        } else if (placement == Placement::TOP_RIGHT || placement == Placement::BOTTOM_RIGHT || placement == Placement::CENTER_RIGHT) {
            out.x = parentSize.x + p.x - s.x;
        } else if (placement == Placement::TOP_CENTER || placement == Placement::BOTTOM_CENTER || placement == Placement::CENTER) {
            out.x = parentSize.x / 2 - s.x / 2 + p.x;
        }
        if (placement == Placement::TOP_LEFT || placement == Placement::TOP_RIGHT || placement == Placement::TOP_CENTER) {
            out.y = parentSize.y + p.y - s.y;
        } else if (placement == Placement::BOTTOM_LEFT || placement == Placement::BOTTOM_RIGHT || placement == Placement::BOTTOM_CENTER) {
            out.y = p.y;
        } else if (placement == Placement::CENTER_LEFT || placement == Placement::CENTER_RIGHT || placement == Placement::CENTER) {
            out.y = parentSize.y / 2 - s.y / 2 + p.y;
        }
        return out;
    }

}
