#pragma once

#include "ui/Layout.hpp"
#include "ui/Shape.hpp"
#include "ui/UIEvent.hpp"
#include "ui/Units.hpp"
#include "utils/Color.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#define BIRDY3D_WIDGET_OPTIONS_STRUCT                                        \
    ::Birdy3d::ui::UIVector position = { 0 };                                \
    ::Birdy3d::ui::UIVector size = { 0 };                                    \
    ::Birdy3d::ui::Placement placement = ::Birdy3d::ui::Placement::TOP_LEFT; \
    bool hidden = false;                                                     \
    std::string name = {};                                                   \
    float weight = 1;                                                        \
    int column = 0;                                                          \
    int row = 0;                                                             \
    Options& merge_widget_options(::Birdy3d::ui::Widget::Options o) {        \
        position = o.position;                                               \
        size = o.size;                                                       \
        placement = o.placement;                                             \
        hidden = o.hidden;                                                   \
        name = o.name;                                                       \
        weight = o.weight;                                                   \
        column = o.column;                                                   \
        row = o.row;                                                         \
        return *this;                                                        \
    }

namespace Birdy3d::ui {

    class Canvas;
    class Rectangle;
    class Text;
    class Theme;
    class Triangle;

    template <class T>
    concept is_widget = std::is_base_of_v<Widget, T>;

    // Is there a better way to implement this?
    template <class T>
    concept is_specialized_widget = std::is_base_of_v<Widget, T> && !std::is_base_of_v<T, Widget>;

    template <class T>
    concept is_layout = std::is_base_of_v<Layout, T>;

    template <class T>
    concept is_widget_options = requires(T t) {
        { t.position } -> std::same_as<UIVector&>;
        { t.size } -> std::same_as<UIVector&>;
        { t.placement } -> std::same_as<Placement&>;
        { t.hidden } -> std::same_as<bool&>;
        { t.name } -> std::same_as<std::string&>;
        { t.weight } -> std::same_as<float&>;
        { t.column } -> std::same_as<int&>;
        { t.row } -> std::same_as<int&>;
    };

    class Widget {
    public:
        struct Options {
            BIRDY3D_WIDGET_OPTIONS_STRUCT
        };

        Widget* parent = nullptr;
        Canvas* canvas = nullptr;

        UIVector position = { 0 };
        UIVector size = { 0 };
        Placement placement = Placement::TOP_LEFT;
        bool hidden = false;
        std::string name = {};

        // Layout specific options
        float weight = 1; ///< Size ratio in DirectionalLayout. 0 means stay on minimum size
        int column = 0; ///< Column in GridLayout
        int row = 0; ///< Row in GridLayout

        Widget(is_widget_options auto options)
            : position(options.position)
            , size(options.size)
            , placement(options.placement)
            , hidden(options.hidden)
            , name(options.name)
            , weight(options.weight)
            , column(options.column)
            , row(options.row) { }

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
        void notify_event(UIEvent&);
        void external_draw();
        bool update_hover();
        void update_visible_area(glm::vec2 parent_visible_top_left, glm::vec2 parent_visible_bottom_right);
        virtual void late_update();
        virtual void on_update();

        // Callbacks
        typedef std::function<void(UIEvent&)> CallbackType;

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
        void execute_callbacks(const std::string& name, UIEvent& event);

        bool has_callbacks(const std::string& name);

        // Events
        virtual void on_scroll(ScrollEvent&);
        virtual void on_click(ClickEvent&);
        virtual void on_key(KeyEvent&);
        virtual void on_char(CharEvent&);
        virtual void on_mouse_enter(MouseEnterEvent&) { }
        virtual void on_mouse_leave(MouseLeaveEvent&) { }
        virtual void on_focus(FocusEvent&) { }
        virtual void on_focus_lost(FocusLostEvent&) { }
        virtual void on_resize(ResizeEvent&) { }
        virtual void on_drop(DropEvent&) { }

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

        template <is_widget T>
        std::shared_ptr<T> add_child(typename T::Options options) {
            auto widget = std::make_shared<T>(options);
            add_child(widget);
            return std::static_pointer_cast<T>(widget);
        }

        template <is_specialized_widget T>
        std::shared_ptr<T> add_child(Options widget_options, typename T::Options specialized_options) {
            return add_child<T>(specialized_options.merge_widget_options(widget_options));
        }

        template <is_widget T = Widget>
        std::shared_ptr<T> get_widget(const std::string& name, bool hidden = true) {
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

    private:
        std::map<std::string, std::vector<CallbackType>> m_callbacks;
    };

}
