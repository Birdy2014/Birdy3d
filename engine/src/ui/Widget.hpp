#ifndef BIRDY3D_WIDGET_HPP
#define BIRDY3D_WIDGET_HPP

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "render/Shader.hpp"
#include "ui/Rectangle.hpp"

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

    Widget *parent = nullptr;
    bool hidden = false;
    glm::vec3 pos;
    float rot;
    glm::vec2 scale;
    Placement placement;

    Widget(Shader *shader, glm::vec3 pos = glm::vec3(0.0f), Placement placement = Placement::TOP_LEFT, float rotation = 0.0f, glm::vec2 scale = glm::vec2(1));
    void addRectangle(glm::ivec2 pos, glm::ivec2 size, glm::vec4 color, float depth = 0.0f);
    void addFilledRectangle(glm::ivec2 pos, glm::ivec2 size, glm::vec4 color, float depth = 0.0f);
    void addChild(Widget *w) {
        w->parent = this;
        children.push_back(w);
    }
    void draw();
    void setOnClick(bool (*clickHandler)()) {
        this->clickHandler = clickHandler;
    }
    bool updateEvents();
    glm::ivec2 getSize();
    glm::vec2 getPos();
    glm::mat4 absTransform(bool normalize = false);

private:
    Shader *shader;
    std::vector<Rectangle> rectangles;
    std::vector<Widget*> children;
    bool (*clickHandler)();

    glm::ivec2 getBottomLeft();
    glm::ivec2 getTopRight();
};

#endif
