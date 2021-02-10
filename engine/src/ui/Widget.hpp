#ifndef BIRDY3D_WIDGET_HPP
#define BIRDY3D_WIDGET_HPP

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "render/Shader.hpp"
#include "ui/Shape.hpp"
#include "ui/TextRenderer.hpp"

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

    enum class Unit {
        PIXELS,
        PERCENT // Not the best name: 1 is 100%
    };

    struct Text {
        glm::vec2 pos;
        float fontSize;
        std::string text;
        glm::vec4 color;
    };

    bool hidden = false;
    glm::vec2 pos;
    float rot;
    Placement placement;

    Widget(glm::vec2 pos = glm::vec2(0.0f), Placement placement = Placement::BOTTOM_LEFT, float rotation = 0.0f);
    void addRectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
    void addFilledRectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
    void addTriangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
    void addFilledTriangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
    void addText(glm::vec2 pos, float fontSize, std::string text, glm::vec4 color);
    void addChild(Widget *w) {
        children.push_back(w);
    }
    void draw();
    void setOnClick(bool (*clickHandler)()) {
        this->clickHandler = clickHandler;
    }
    bool updateEvents();
    glm::ivec2 getSize();
    glm::vec2 getPos(glm::vec2 parentSize);
    virtual void arrange(glm::mat4 move, glm::vec2 size);

protected:
    std::vector<Shape*> shapes;
    std::vector<Widget::Text> texts;
    std::vector<Widget*> children;
    bool (*clickHandler)(); // FIXME: This will cause a segfault if not set
    glm::mat4 move;
    glm::vec2 size;

    glm::ivec2 getBottomLeft();
    glm::ivec2 getTopRight();
    glm::mat4 normalizedMove();
};

#endif
