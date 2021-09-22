#pragma once

#include "ui/Shape.hpp"

namespace Birdy3d {

    class Triangle : public Shape {
    public:
        Triangle(UIVector position, UIVector size, Color color = Color::WHITE, Type type = FILLED, Placement placement = Placement::BOTTOM_LEFT);
        ~Triangle();
        void draw(glm::mat4 move) override;
        bool contains(glm::vec2 point) override;

    private:
        void create_buffers();
        void update_values();
        float area(glm::vec2 a, glm::vec2 b, glm::vec2 c);
    };

}
