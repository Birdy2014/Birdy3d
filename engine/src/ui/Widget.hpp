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

    template <class T>
    concept is_widget = std::is_base_of_v<Widget, T>;
    template <class T>
    concept is_layout = std::is_base_of_v<Layout, T>;

    class Widget {
    public:
        struct Options {
            UIVector pos = { 0 };
            UIVector size = { 0 };
            Placement placement = Placement::TOP_LEFT;
            bool hidden = false;
            std::string name = {};

            // Layout specific options
            float weight = 1; ///< Size ratio in DirectionalLayout. 0 means stay on minimum size
            int column = 0; ///< Column in GridLayout
            int row = 0; ///< Row in GridLayout
        };

        enum class EventType {
            SCROLL,
            CLICK,
            KEY,
            CHAR,
            MOUSE_ENTER,
            MOUSE_LEAVE,
            FOCUS,
            FOCUS_LOST,
            RESIZE,
        };

        Widget* parent = nullptr;
        Canvas* canvas = nullptr;
        Options options;

        Widget(Options);
        virtual ~Widget() = default;

        void to_foreground(Widget* w);

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
        void notify_event(EventType, events::Event const*);
        void external_draw();
        bool update_hover(bool hover);
        void update_visible_area(glm::vec2 parent_visible_top_left, glm::vec2 parent_visible_bottom_right);
        virtual void late_update();
        virtual void on_update();

        // Callbacks
        typedef std::function<void(std::any)> CallbackType;

        void add_callback(const std::string& name, CallbackType callback);

    protected:
        friend class Canvas;

        std::vector<std::unique_ptr<Shape>> m_shapes;
        std::list<std::shared_ptr<Widget>> m_children;
        std::unique_ptr<Layout> m_layout = nullptr;
        glm::vec2 m_actual_size = glm::vec2(1);
        glm::vec2 m_actual_pos = glm::vec2(1);
        glm::vec2 m_visible_size = glm::vec2(1);
        glm::vec2 m_visible_pos = glm::vec2(1);
        glm::mat4 m_move = glm::mat4(1);
        glm::vec4 m_padding = glm::vec4(0); ///< left, right, up, down
        bool m_children_visible = true;
        bool m_shapes_visible = true;

        /**
         * @brief Custom drawing.
         *
         * Override this function for any drawing additional to shapes and child widgets.
         */
        virtual void draw();

        virtual bool contains(glm::vec2) const;

        // Callbacks
        void execute_callbacks(const std::string& name, std::any = {});
        bool has_callbacks(const std::string& name);

        // Events
        virtual bool on_scroll(const events::InputScrollEvent&);
        virtual bool on_click(const events::InputClickEvent&);
        virtual bool on_key(const events::InputKeyEvent&);
        virtual bool on_char(const events::InputCharEvent&);
        virtual void on_mouse_enter() { }
        virtual void on_mouse_leave() { }
        virtual void on_focus() { }
        virtual void on_focus_lost() { }
        virtual void on_resize() { }
        virtual void on_drop(const std::any&) { }

        // Shapes
        Rectangle* add_rectangle(UIVector pos, UIVector size, utils::Color::Name, Placement = Placement::TOP_LEFT);
        Rectangle* add_filled_rectangle(UIVector pos, UIVector size, utils::Color::Name, Placement = Placement::TOP_LEFT);
        Triangle* add_triangle(UIVector pos, UIVector size, utils::Color::Name, Placement = Placement::TOP_LEFT);
        Triangle* add_filled_triangle(UIVector pos, UIVector size, utils::Color::Name, Placement = Placement::TOP_LEFT);
        Text* add_text(UIVector pos, std::string text, utils::Color::Name, Placement = Placement::TOP_LEFT, float font_size = 0);

        // Layout
        template <is_layout T, typename... Args>
        void set_layout(Args... args) {
            m_layout = std::make_unique<T>(args...);
        }

        void unset_layout() { m_layout = nullptr; }

        void add_child(std::shared_ptr<Widget>);
        void clear_children();

        template <is_widget T, typename... Args>
        std::shared_ptr<T> add_child(Options options, Args... args) {
            auto widget = std::make_shared<T>(options, args...);
            add_child(widget);
            return std::static_pointer_cast<T>(widget);
        }

        template <is_widget T = Widget>
        std::shared_ptr<T> get_widget(const std::string& name, bool hidden = true) {
            if (this->options.hidden && !hidden)
                return nullptr;
            for (const auto& child : m_children) {
                std::shared_ptr<T> casted = std::dynamic_pointer_cast<T>(child);
                if (casted && child->options.name == name) {
                    return casted;
                }
                std::shared_ptr<T> result = child->get_widget<T>(name, hidden);
                if (result)
                    return result;
            }
            return nullptr;
        }

    private:
        std::map<std::string, std::vector<CallbackType>> m_callbacks;
    };

}
