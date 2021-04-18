#pragma once

#include "core/Application.hpp"
#include "ui/Utils.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Birdy3d {

    class Shape;
    class Theme;

    class Widget {
    public:
        bool hidden = false;
        UIVector pos;
        UIVector size;
        float rot;
        Placement placement;
        Theme* theme;

        Widget(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), Placement placement = Placement::BOTTOM_LEFT, float rotation = 0.0f, Theme* theme = Application::defaultTheme);
        void addRectangle(UIVector pos, UIVector size, Color color, Placement placement = Placement::BOTTOM_LEFT);
        void addFilledRectangle(UIVector pos, UIVector size, Color color, Placement placement = Placement::BOTTOM_LEFT);
        void addTriangle(UIVector pos, UIVector size, Color color);
        void addFilledTriangle(UIVector pos, UIVector size, Color color);
        void addText(UIVector pos, float fontSize, std::string text, Color color, Placement placement);
        virtual void draw();
        virtual bool updateEvents();

        // Returns the position relative to the parent's origin in pixels
        glm::vec2 preferredPosition(glm::vec2 parentSize);

        // Returns the size in pixels
        virtual glm::vec2 minimalSize();
        virtual glm::vec2 preferredSize(glm::vec2 parentSize);

        virtual void arrange(glm::mat4 move, glm::vec2 size);

        bool isHovering() { return hover; }

        template<class T>
        T* getShape(std::string name = "") {
            for (Shape* s : this->shapes) {
                if (name != "" && name != s->name)
                    continue;
                T* casted = dynamic_cast<T*>(s);
                if (casted)
                    return casted;
            }
            return nullptr;
        }

    protected:
        std::vector<Shape*> shapes;
        glm::mat4 move;
        glm::vec2 actualSize;
        bool hover = false;

        glm::mat4 normalizedMove();
    };

}
