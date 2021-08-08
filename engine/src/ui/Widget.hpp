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
        float weight = 1; // Size ratio in DirectionalLayout. 0 means stay on minimum size
        Widget* parent = nullptr;
        Canvas* canvas = nullptr;

        Widget(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), Placement placement = Placement::BOTTOM_LEFT, std::string name = "");
        virtual ~Widget() { }
        virtual void draw();

        // Layout
        template <class T, typename... Args>
        void set_layout(Args... args) {
            static_assert(std::is_base_of<Layout, T>::value);
            m_layout = std::make_unique<T>(args...);
        }

        void unset_layout() { m_layout = nullptr; }

        void add_child(std::shared_ptr<Widget>);

        template <class T, typename... Args>
        std::shared_ptr<T> add_child(Args... args) {
            static_assert(std::is_base_of<Widget, T>::value);
            auto widget = std::make_shared<T>(args...);
            add_child(widget);
            return std::static_pointer_cast<T>(widget);
        }

        void toForeground(Widget* w);

        template <class T = Widget>
        std::shared_ptr<T> get_widget(const std::string& name, bool hidden = true) {
            static_assert(std::is_base_of<Widget, T>::value);
            if (this->hidden && !hidden)
                return nullptr;
            for (const auto& child : m_children) {
                std::shared_ptr<T> casted = std::dynamic_pointer_cast<T>(child);
                if (casted && child->name == name) {
                    return casted;
                }
                std::shared_ptr<T> result = child->get_widget<T>(name, hidden);
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

        bool is_hovering();
        bool is_focused();
        void focus();
        void grab_cursor();
        void ungrab_cursor();

        // Getters
        glm::vec2 actual_pos() { return m_actual_pos; }
        glm::vec2 actual_size() { return m_actual_size; }

        // External Event calls
        virtual bool update_hover(bool hover);
        virtual void late_update();
        virtual void on_update();

    protected:
        friend class Canvas;

        std::vector<std::unique_ptr<Shape>> m_shapes;
        std::list<std::shared_ptr<Widget>> m_children;
        std::unique_ptr<Layout> m_layout = std::make_unique<EmptyLayout>();
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
