#pragma once

#include "core/Logger.hpp"
#include <glm/glm.hpp>

namespace Birdy3d {

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

    class Unit {
    public:
        float pixels;
        float percent;

        Unit(float pixels, float percent = 0.0f);
        void operator=(float value);
        operator float();
        float toPixels(float parentSize = 0);
        Unit operator+(const Unit& other);
        Unit& operator+=(const Unit& other);
        Unit operator+(const float other);
        Unit& operator+=(const float other);
        Unit operator-();
        Unit operator-(const Unit& other);
        Unit& operator-=(const Unit& other);
    };

    Unit operator"" _px(long double value);
    Unit operator"" _px(unsigned long long value);
    Unit operator"" _p(long double value);
    Unit operator"" _p(unsigned long long value);

    class UIVector {
    public:
        Unit x;
        Unit y;

        UIVector();
        UIVector(const UIVector& v);
        UIVector(const glm::vec2& v);
        UIVector(Unit x);
        UIVector(Unit x, Unit y);
        UIVector& operator=(const UIVector& other);
        UIVector operator+(const UIVector& other);
        UIVector operator+(const float other);
        UIVector operator-();
        UIVector operator-(const UIVector& other);
        glm::vec2 toPixels(glm::vec2 parentSize = glm::vec2(0));
        operator glm::vec2();
    };

    class Color {
    public:
        static Color WHITE;
        static Color BLACK;

        Color();
        Color(const std::string& color);
        Color(const char* color);
        Color(glm::vec4 color);
        operator glm::vec4();
        Color& operator=(const std::string& color);
        static glm::vec4 parse(const std::string& colorString);

    private:
        glm::vec4 value;
    };

    class Utils {
    public:
        static glm::vec2 getRelativePosition(UIVector pos, UIVector size, glm::vec2 parentSize, Placement placement);
    };

}
