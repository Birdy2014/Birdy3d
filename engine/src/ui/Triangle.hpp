#ifndef BIRDY3D_TRIANGLE_HPP
#define BIRDY3D_TRIANGLE_HPP

#include "ui/Shape.hpp"

class Triangle : public Shape {
public:
    Triangle(glm::vec2 position, glm::vec2 size, glm::vec4 color = glm::vec4(1), Type type = FILLED);
    void draw(glm::mat4 move) override;
    bool contains(glm::vec2 point) override;

private:
    void createBuffers();
    void updateVBO();
    float area(glm::vec2 a, glm::vec2 b, glm::vec2 c);
};

#endif
