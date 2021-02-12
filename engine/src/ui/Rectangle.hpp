#ifndef BIRDY3D_RECTANGLE_HPP
#define BIRDY3D_RECTANGLE_HPP

#include <glm/glm.hpp>
#include <glad/glad.h>
#include "render/Shader.hpp"
#include "ui/Shape.hpp"

class Rectangle : public Shape {
public:
    Rectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color = glm::vec4(1), Type type = FILLED, Placement placement = Placement::BOTTOM_LEFT, Unit unit = Unit::PIXELS);
    void draw(glm::mat4 move) override;
    bool contains(glm::vec2 point) override;

private:
    void createBuffers();
    void updateVBO();
};

#endif
