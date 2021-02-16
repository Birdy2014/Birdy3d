#pragma once

#include "ui/Shape.hpp"

namespace Birdy3d {

    class Triangle : public Shape {
    public:
        Triangle(glm::vec2 position, glm::vec2 size, glm::vec4 color = glm::vec4(1), Type type = FILLED, Placement placement = Placement::BOTTOM_LEFT, Unit unit = Unit::PIXELS);
        void draw(glm::mat4 move) override;
        bool contains(glm::vec2 point) override;

    private:
        void createBuffers();
        void updateVBO();
        float area(glm::vec2 a, glm::vec2 b, glm::vec2 c);
    };

}
