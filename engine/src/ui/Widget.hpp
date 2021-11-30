#pragma once

#include "core/Application.hpp"
#include "events/InputEvents.hpp"
#include "ui/Layout.hpp"
#include "ui/Shape.hpp"
#include "ui/Units.hpp"
#include "utils/Color.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Birdy3d::ui {

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
        virtual ~Widget() = default;

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

        void to_foreground(Widget* w);

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
        glm::vec2 preferred_position(glm::vec2 parentSize, glm::vec2 size);

        // Returns the size in pixels
        virtual glm::vec2 minimal_size();
        glm::vec2 preferred_size(glm::vec2 parentSize);

        virtual void arrange(glm::vec2 pos, glm::vec2 size);
        void set_canvas(Canvas*);

        bool is_hovering();
        bool is_focused();
        bool was_last_focused();
        void focus();
        void grab_cursor();
        void ungrab_cursor();

        // Getters
        glm::vec2 actual_pos() { return m_actual_pos; }
        glm::vec2 actual_size() { return m_actual_size; }

        // External Event calls
        void external_draw();
        bool update_hover(bool hover);
        void update_visible_area(glm::vec2 parent_visible_bottom_left, glm::vec2 parent_visible_top_right);
        virtual void late_update();
        virtual void on_update();

        // Callbacks
        typedef std::function<void()> CallbackType;

        void add_callback(const std::string& name, CallbackType callback);

    protected:
        friend class Canvas;

        std::vector<std::unique_ptr<Shape>> m_shapes;
        std::list<std::shared_ptr<Widget>> m_children;
        std::unique_ptr<Layout> m_layout = std::make_unique<EmptyLayout>();
        glm::vec2 m_actual_size = glm::vec2(1);
        glm::vec2 m_actual_pos = glm::vec2(1);
        glm::vec2 m_visible_size = glm::vec2(1);
        glm::vec2 m_visible_pos = glm::vec2(1);
        glm::mat4 m_move = glm::mat4(1);
        glm::vec4 m_padding = glm::vec4(0); // left, right, down, up
        bool m_children_visible = true;

        virtual void draw();
        virtual bool contains(glm::vec2) const;

        // Callbacks
        void execute_callbacks(const std::string& name);
        bool has_callbacks(const std::string& name);

        // Events
        virtual void on_scroll(const events::InputScrollEvent& event);
        virtual void on_click(const events::InputClickEvent& event);
        virtual void on_key(const events::InputKeyEvent& event);
        virtual void on_char(const events::InputCharEvent& event);
        virtual void on_mouse_enter() { }
        virtual void on_mouse_leave() { }
        virtual void on_focus() { }
        virtual void on_focus_lost() { }
        virtual void on_resize() { }

        // Shapes
        Rectangle* add_rectangle(UIVector pos, UIVector size, utils::Color::Name, Placement = Placement::BOTTOM_LEFT);
        Rectangle* add_filled_rectangle(UIVector pos, UIVector size, utils::Color::Name, Placement = Placement::BOTTOM_LEFT);
        Triangle* add_triangle(UIVector pos, UIVector size, utils::Color::Name, Placement = Placement::BOTTOM_LEFT);
        Triangle* add_filled_triangle(UIVector pos, UIVector size, utils::Color::Name, Placement = Placement::BOTTOM_LEFT);
        Text* add_text(UIVector pos, std::string text, utils::Color::Name, Placement = Placement::BOTTOM_LEFT, float font_size = 0);

    private:
        bool m_hovered_last_frame = false;
        std::map<std::string, std::vector<CallbackType>> m_callbacks;
    };

}
