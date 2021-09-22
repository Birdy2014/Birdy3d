#pragma once

#include "core/Base.hpp"

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
        bool operator==(const Unit& other);
        Unit operator+(const Unit& other);
        Unit& operator+=(const Unit& other);
        Unit operator+(const float other);
        Unit& operator+=(const float other);
        Unit operator+(const int other);
        Unit& operator+=(const int other);
        Unit operator-();
        Unit operator-(const Unit& other);
        Unit& operator-=(const Unit& other);
        Unit operator-(const float other);
        Unit& operator-=(const float other);
        Unit operator-(const int other);
        Unit& operator-=(const int other);
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
        bool operator==(const UIVector& other);
        UIVector operator+(const UIVector& other);
        UIVector operator+(const float other);
        UIVector operator-();
        UIVector operator-(const UIVector& other);
        UIVector operator+=(const UIVector& other);
        UIVector operator+=(const float other);
        UIVector operator-=(const UIVector& other);
        glm::vec2 toPixels(glm::vec2 parentSize = glm::vec2(0));
        operator glm::vec2();
    };

    class Utils {
    public:
        static glm::vec2 getRelativePosition(UIVector pos, UIVector size, glm::vec2 parentSize, Placement placement);
    };

    struct UIVertex {
        glm::vec2 position;
        glm::vec2 texcoords;
    };

}
