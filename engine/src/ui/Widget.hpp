#pragma once

#include "ui/Utils.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Birdy3d {

    class Shape;
    class Text;

    class Widget {
    public:
        bool hidden = false;
        glm::vec2 pos;
        glm::vec2 size;
        float rot;
        Placement placement;
        Unit unit;

        Widget(glm::vec2 pos = glm::vec2(0.0f), glm::vec2 size = glm::vec2(0.0f), Placement placement = Placement::BOTTOM_LEFT, Unit unit = Unit::PIXELS, float rotation = 0.0f);
        void addRectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color, Placement placement = Placement::BOTTOM_LEFT, Unit unit = Unit::PIXELS);
        void addFilledRectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color, Placement placement = Placement::BOTTOM_LEFT, Unit unit = Unit::PIXELS);
        void addTriangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
        void addFilledTriangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
        void addText(glm::vec2 pos, float fontSize, std::string text, glm::vec4 color, Placement placement);
        virtual void draw();
        void setOnClick(bool (*clickHandler)()) {
            this->clickHandler = clickHandler;
        }
        virtual bool updateEvents();

        // Returns the position relative to the parent's origin in pixels
        glm::vec2 preferredPosition(glm::vec2 parentSize);

        // Returns the size in pixels
        virtual glm::vec2 minimalSize();
        virtual glm::vec2 preferredSize(glm::vec2 parentSize);

        virtual void arrange(glm::mat4 move, glm::vec2 size);

    protected:
        std::vector<Shape*> shapes;
        std::vector<Text*> texts;
        bool (*clickHandler)(); // FIXME: This will cause a segfault if not set
        glm::mat4 move;

        glm::mat4 normalizedMove();
    };

}
