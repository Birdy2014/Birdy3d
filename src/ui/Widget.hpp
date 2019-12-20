#ifndef BIRDY3D_WIDGET_HPP
#define BIRDY3D_WIDGET_HPP

#include <vector>
#include "../render/Shader.hpp"
#include "UI_Vertex.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

enum Placement {
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

class Widget {
public:
    bool hidden = false;

    Widget(glm::vec3 pos = glm::vec3(0.0f), Placement placement = TOP_LEFT) {
        this->pos = pos;
        this->placement = placement;
    }
    void addLine(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 color);
    void addTriangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 color);
    void addRectangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 color);
    void addRectangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec3 color, float depth = 0.0f) {
        addRectangle(glm::vec3(pos1, depth), glm::vec3(pos2, depth), color);
    }
    void addChild(Widget w) {
        children.push_back(w);
    }
    void draw(Shader shader, glm::mat4 move, float parentSize[2]);
    void draw(Shader shader) {
        draw(shader, glm::mat4(1.0f), getViewportSize());
    }
    void fillBuffer();
    void setOnClick(void (*clickHandler)()) {
        this->clickHandler = clickHandler;
    }
    void updateEvents(GLFWwindow *window, glm::vec3 parentAbsPos, float parentSize[]);
    void updateEvents(GLFWwindow *window) {
        updateEvents(window, glm::vec3(0.0f), getViewportSize());
    }
    glm::vec3 getAbsPos(float parentSize[]);

private:
    unsigned int lines_VBO, lines_VAO, triangles_VBO, triangles_VAO;
    float x, y;
    glm::vec3 pos;
    std::vector<UI_Vertex> lines;
    std::vector<UI_Vertex> triangles;
    std::vector<Widget> children;
    void (*clickHandler)();
    Placement placement;

    float *getSize();
    float *getViewportSize();
};

#endif