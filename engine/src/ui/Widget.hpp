#pragma once

#include "core/Application.hpp"
#include "events/InputEvents.hpp"
#include "render/Color.hpp"
#include "ui/Layout.hpp"
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
        virtual ~Widget() { }
        virtual void draw();

        // Layout
        template <class T, typename... Args>
        void set_layout(Args... args) {
            static_assert(std::is_base_of<Layout, T>::value);
            m_layout = std::make_unique<T>(args...);
        }

        void unset_layout() { m_layout = nullptr; }

        void add_child(std::unique_ptr<Widget>);

        template <class T, typename... Args>
        T* add_child(Args... args) {
            static_assert(std::is_base_of<Widget, T>::value);
            std::unique_ptr<Widget> widget = std::make_unique<T>(args...);
            Widget* widget_ptr = widget.get();
            add_child(std::move(widget));
            return static_cast<T*>(widget_ptr);
        }

        void toForeground(Widget* w);

        template <class T = Widget>
        T* get_widget(const std::string& name, bool hidden = true) {
            static_assert(std::is_base_of<Widget, T>::value);
            if (this->hidden && !hidden)
                return nullptr;
            for (const std::unique_ptr<Widget>& child : m_children) {
                T* casted = dynamic_cast<T*>(child.get());
                if (casted && child->name == name) {
                    return casted;
                }
                T* result = child->get_widget<T>(name, hidden);
                if (result)
                    return result;
            }
            return nullptr;
        }

        // Returns the position relative to the parent's origin in pixels
        glm::vec2 preferredPosition(glm::vec2 parentSize, glm::vec2 size);

        // Returns the size in pixels
        virtual glm::vec2 minimalSize();
        glm::vec2 preferredSize(glm::vec2 parentSize);

        virtual void arrange(glm::vec2 pos, glm::vec2 size);
        void set_canvas(Canvas*);

        template <class T = Widget>
        T* get_shape(std::string name = "") const {
            static_assert(std::is_base_of<Widget, T>::value);
            for (const auto& s : m_shapes) {
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
        virtual void on_update();

    protected:
        friend class Canvas;

        std::vector<std::unique_ptr<Shape>> m_shapes;
        std::list<std::unique_ptr<Widget>> m_children;
        std::unique_ptr<Layout> m_layout = nullptr;
        glm::vec2 m_actual_size = glm::vec2(1);
        glm::vec2 m_actual_pos = glm::vec2(1);
        glm::vec4 m_padding = glm::vec4(0); // left, right, down, up

        glm::mat4 normalizedMove();
        virtual bool contains(glm::vec2) const;

        // Events
        virtual void on_scroll(InputScrollEvent* event) { }
        virtual void on_click(InputClickEvent* event) { }
        virtual void on_key(InputKeyEvent* event) { }
        virtual void on_char(InputCharEvent* event) { }
        virtual void on_mouse_enter() { }
        virtual void on_mouse_leave() { }
        virtual void on_focus() { }
        virtual void on_focus_lost() { }

        // Shapes
        Rectangle* add_rectangle(UIVector pos, UIVector size, Color color, Placement placement = Placement::BOTTOM_LEFT);
        Rectangle* add_filled_rectangle(UIVector pos, UIVector size, Color color, Placement placement = Placement::BOTTOM_LEFT);
        Triangle* add_triangle(UIVector pos, UIVector size, Color color);
        Triangle* add_filled_triangle(UIVector pos, UIVector size, Color color);
        Text* add_text(UIVector pos, float fontSize, std::string text, Color color, Placement placement);

    private:
        bool m_hovered_last_frame = false;
    };

}
