#pragma once

#include "core/Application.hpp"
#include "events/InputEvents.hpp"
#include "render/Color.hpp"
#include "ui/Shape.hpp"
#include "ui/Utils.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Birdy3d {

    class Canvas;
    class Rectangle;
    class Text;
    class Theme;
    class Triangle;

    class Widget {
    public:
        std::string name;
        bool hidden = false;
        UIVector pos;
        UIVector size;
        Placement placement;
        Theme* theme;
        float weight = 1; // Size ratio in DirectionalLayout. 0 means stay on minimum size
        Widget* parent = nullptr;
        Canvas* canvas = nullptr;

        Widget(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), Placement placement = Placement::BOTTOM_LEFT, Theme* theme = Application::defaultTheme, std::string name = "");
        virtual ~Widget();
        Rectangle* addRectangle(UIVector pos, UIVector size, Color color, Placement placement = Placement::BOTTOM_LEFT);
        Rectangle* addFilledRectangle(UIVector pos, UIVector size, Color color, Placement placement = Placement::BOTTOM_LEFT);
        Triangle* addTriangle(UIVector pos, UIVector size, Color color);
        Triangle* addFilledTriangle(UIVector pos, UIVector size, Color color);
        Text* addText(UIVector pos, float fontSize, std::string text, Color color, Placement placement);
        virtual void draw();

        // Returns the position relative to the parent's origin in pixels
        glm::vec2 preferredPosition(glm::vec2 parentSize, glm::vec2 size);

        // Returns the size in pixels
        virtual glm::vec2 minimalSize();
        glm::vec2 preferredSize(glm::vec2 parentSize);

        virtual void arrange(glm::vec2 pos, glm::vec2 size);
        virtual void set_canvas(Canvas*);

        template <class T>
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

        bool is_hovering();
        bool is_focused();
        void focus();
        void grab_cursor();
        void ungrab_cursor();

        // External Event calls
        virtual bool update_hover(bool hover);
        virtual void late_update();
        virtual void on_update() { }

    protected:
        friend class Canvas;

        std::vector<Shape*> shapes;
        glm::vec2 actualSize = glm::vec2(1);
        glm::vec2 actualPos = glm::vec2(1);

        glm::mat4 normalizedMove();

        // Events
        virtual void on_scroll(InputScrollEvent* event) { }
        virtual void on_click(InputClickEvent* event) { }
        virtual void on_key(InputKeyEvent* event) { }
        virtual void on_char(InputCharEvent* event) { }
        virtual void on_mouse_enter() { }
        virtual void on_mouse_leave() { }
        virtual void on_focus() { }
        virtual void on_focus_lost() { }

    private:
        bool m_hovered_last_frame = false;
    };

}
