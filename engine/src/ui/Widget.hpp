#ifndef BIRDY3D_WIDGET_HPP
#define BIRDY3D_WIDGET_HPP

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "render/Shader.hpp"
#include "ui/UI_Vertex.hpp"

class Widget {
public:
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

    bool hidden = false;

    Widget(Shader *shader, glm::vec3 pos = glm::vec3(0.0f), Placement placement = Placement::TOP_LEFT);
    void addLine(glm::vec2 pos1, glm::vec2 pos2, glm::vec3 color, float depth = 0.0f, float opacity = 1.0f);
    void addTriangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec3 color, float depth = 0.0f, float opacity = 1.0f);
    void addRectangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec3 color, float depth = 0.0f, float opacity = 1.0f);
    void addFilledRectangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec3 color, float depth = 0.0f, float opacity = 1.0f);
    void addChild(Widget w) {
        children.push_back(w);
    }
    void draw(glm::mat4 move, glm::vec2 parentSize);
    void draw();
    void fillBuffer();
    void setOnClick(void (*clickHandler)()) {
        this->clickHandler = clickHandler;
    }
    void updateEvents(glm::vec3 parentAbsPos, glm::vec2 parentSize);
    void updateEvents();
    glm::vec3 getAbsPos(int parentWidth, int parentHeight);

private:
    Shader *shader;
    unsigned int lines_VBO, lines_VAO, triangles_VBO, triangles_VAO;
    float x, y;
    glm::vec3 pos;
    std::vector<UI_Vertex> lines;
    std::vector<UI_Vertex> triangles;
    std::vector<Widget> children;
    void (*clickHandler)();
    Placement placement;

    glm::vec2 getSize();
};

#endif
