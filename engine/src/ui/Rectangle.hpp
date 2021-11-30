#pragma once

#include "ui/Shape.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Birdy3d::ui {

    class Rectangle : public Shape {
    public:
        Rectangle(UIVector pos, UIVector size, utils::Color::Name color = utils::Color::Name::WHITE, Type type = FILLED, Placement placement = Placement::BOTTOM_LEFT);
        ~Rectangle();
        void draw(glm::mat4 move) override;
        bool contains(glm::vec2 point) override;

    private:
        void create_buffers();
        void update_values();
    };

}
