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

    struct Text {
        glm::vec2 pos;
        float fontSize;
        std::string text;
        glm::vec4 color;
    };

    Widget *parent = nullptr;
    bool hidden = false;
    glm::vec3 pos;
    float rot;
    glm::vec2 scale;
    Placement placement;

    Widget(glm::vec3 pos = glm::vec3(0.0f), Placement placement = Placement::TOP_LEFT, float rotation = 0.0f, glm::vec2 scale = glm::vec2(1));
    void addRectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
    void addFilledRectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
    void addTriangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
    void addFilledTriangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
    void addText(glm::vec2 pos, float fontSize, std::string text, glm::vec4 color);
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
    std::vector<Shape*> shapes;
    std::vector<Widget::Text> texts;
    std::vector<Widget*> children;
    bool (*clickHandler)();

    glm::ivec2 getBottomLeft();
    glm::ivec2 getTopRight();
};

#endif
