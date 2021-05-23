#include "ui/Utils.hpp"

namespace Birdy3d {

    Unit::Unit(float pixels, float percent)
        : pixels(pixels)
        , percent(percent) { }

    void Unit::operator=(float value) {
        this->pixels = value;
    }

    Unit::operator float() {
        return pixels;
    }

    float Unit::toPixels(float parentSize) {
        return (percent / 100) * parentSize + pixels;
    }

    Unit Unit::operator+(const Unit& other) {
        return Unit(pixels + other.pixels, percent + other.percent);
    }

    Unit& Unit::operator+=(const Unit& other) {
        pixels += other.pixels;
        percent += other.percent;
        return *this;
    }

    Unit Unit::operator+(const float other) {
        return Unit(pixels + other, percent);
    }

    Unit& Unit::operator+=(const float other) {
        pixels += other;
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

    glm::vec2 UIVector::toPixels(glm::vec2 parentSize) {
        return glm::vec2(x.toPixels(parentSize.x), y.toPixels(parentSize.y));
    }

    UIVector::operator glm::vec2() {
        return glm::vec2(x, y);
    }

    Color::Color() {
        value = glm::vec4(0, 0, 0, 1);
    }

    Color::Color(const std::string& color) {
        value = parse(color);
    }

    Color::Color(const char* color) {
        value = parse(std::string(color));
    }

    Color::Color(glm::vec4 color) {
        value = color;
    }

    Color::operator glm::vec4() {
        return value;
    }

    Color& Color::operator=(const std::string& color) {
        value = parse(color);
        return *this;
    }

    glm::vec4 Color::parse(const std::string& colorString) {
        bool hasHash = false;
        if (colorString.at(0) == '#')
            hasHash = true;

        size_t colorLength;
        if (colorString.length() == 6 + hasHash || colorString.length() == 8 + hasHash) {
            colorLength = 2;
        } else if (colorString.length() == 3 + hasHash) {
            colorLength = 1;
        } else {
            Logger::warn("Invalid color: " + colorString);
            return glm::vec4(1);
        }

        glm::vec4 color(1);

        for (size_t pos = hasHash; pos < colorString.length(); pos += colorLength) {
            int nr = std::stoi(colorString.substr(pos, colorLength), nullptr, 16);
            color[pos / colorLength] = nr / std::pow(16, colorLength);
        }
        return color;
    }

    Color Color::WHITE = Color("#FFFFFFFF");
    Color Color::BLACK = Color("#000000FF");

    glm::vec2 Utils::getRelativePosition(UIVector pos, UIVector size, glm::vec2 parentSize, Placement placement) {
        glm::vec2 p = pos.toPixels(parentSize);
        glm::vec2 s = size.toPixels(parentSize);
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
