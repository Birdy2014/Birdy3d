#pragma once

#include "ui/Shape.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Birdy3d {

    class Rectangle : public Shape {
    public:
        Rectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color = glm::vec4(1), Type type = FILLED, Placement placement = Placement::BOTTOM_LEFT, Unit unit = Unit::PIXELS);
        void draw(glm::mat4 move) override;
        bool contains(glm::vec2 point) override;

    private:
        void createBuffers();
        void updateVBO();
    };

}
