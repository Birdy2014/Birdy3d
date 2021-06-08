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
        enum class EventType {
            UPDATE,
            SCROLL,
            CLICK,
            KEY,
            CHAR
        };

        std::string name;
        bool hidden = false;
        UIVector pos;
        UIVector size;
        Placement placement;
        Theme* theme;
        float weight = 1; // Size ratio in DirectionalLayout. 0 means stay on minimum size
        Widget* parent;

        Widget(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), Placement placement = Placement::BOTTOM_LEFT, Theme* theme = Application::defaultTheme, std::string name = "");
        virtual ~Widget();
        void addRectangle(UIVector pos, UIVector size, Color color, Placement placement = Placement::BOTTOM_LEFT);
        void addFilledRectangle(UIVector pos, UIVector size, Color color, Placement placement = Placement::BOTTOM_LEFT);
        void addTriangle(UIVector pos, UIVector size, Color color);
        void addFilledTriangle(UIVector pos, UIVector size, Color color);
        void addText(UIVector pos, float fontSize, std::string text, Color color, Placement placement);
        virtual void draw();

        // Returns the position relative to the parent's origin in pixels
        glm::vec2 preferredPosition(glm::vec2 parentSize, glm::vec2 size);

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
        bool notifyEvent(EventType type, Event* event, bool hover);
        virtual void lateUpdate();

    protected:
        std::vector<Shape*> shapes;
        glm::vec2 actualSize = glm::vec2(1);
        glm::vec2 actualPos = glm::vec2(1);

        glm::mat4 normalizedMove();

        // Events
        virtual bool update(bool hover);
        virtual bool onScroll(InputScrollEvent* event, bool hover);
        virtual bool onClick(InputClickEvent* event, bool hover);
        virtual bool onKey(InputKeyEvent* event, bool hover);
        virtual bool onChar(InputCharEvent* event, bool hover);
        virtual void onMouseEnter();
        virtual void onMouseLeave();

    private:
        bool hoveredLastFrame = false;
        bool runMouseEnter = false;
    };

}
