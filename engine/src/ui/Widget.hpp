#pragma once

#include "core/Application.hpp"
#include "events/InputEvents.hpp"
#include "ui/Utils.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Birdy3d {

    class Shape;
    class Theme;

    class Widget {
    public:
        std::string name;
        bool hidden = false;
        UIVector pos;
        UIVector size;
        float rot;
        Placement placement;
        Theme* theme;
        float weight = 1; // Size ratio in DirectionalLayout. 0 means stay on minimum size

        Widget(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), Placement placement = Placement::BOTTOM_LEFT, float rotation = 0.0f, Theme* theme = Application::defaultTheme, std::string name = "");
        virtual ~Widget();
        void addRectangle(UIVector pos, UIVector size, Color color, Placement placement = Placement::BOTTOM_LEFT);
        void addFilledRectangle(UIVector pos, UIVector size, Color color, Placement placement = Placement::BOTTOM_LEFT);
        void addTriangle(UIVector pos, UIVector size, Color color);
        void addFilledTriangle(UIVector pos, UIVector size, Color color);
        void addText(UIVector pos, float fontSize, std::string text, Color color, Placement placement);
        virtual void draw();
        virtual void update();

        // Returns the position relative to the parent's origin in pixels
        glm::vec2 preferredPosition(glm::vec2 parentSize);

        // Returns the size in pixels
        virtual glm::vec2 minimalSize();
        virtual glm::vec2 preferredSize(glm::vec2 parentSize);

        virtual void arrange(glm::vec2 pos, glm::vec2 size);

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

        // External Event calls
        bool _onScroll(InputScrollEvent* event, bool hover);
        bool _onClick(InputClickEvent* event, bool hover);
        bool _onKey(InputKeyEvent* event, bool hover);
        bool _onChar(InputCharEvent* event, bool hover);

    protected:
        std::vector<Shape*> shapes;
        glm::vec2 actualSize = glm::vec2(1);
        glm::vec2 actualPos = glm::vec2(1);

        glm::mat4 normalizedMove();

        // Events
        virtual bool onScroll(InputScrollEvent* event, bool hover);
        virtual bool onClick(InputClickEvent* event, bool hover);
        virtual bool onKey(InputKeyEvent* event, bool hover);
        virtual bool onChar(InputCharEvent* event, bool hover);
    };

}
