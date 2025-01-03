#pragma once

#include "render/Forward.hpp"
#include "ui/Forward.hpp"
#include "ui/Layout.hpp"
#include "ui/Rect.hpp"
#include "ui/Text.hpp"
#include "ui/UIEvent.hpp"
#include "ui/Units.hpp"
#include "utils/Color.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#define BIRDY3D_WIDGET_OPTIONS_STRUCT                                        \
    ::Birdy3d::ui::Position position = {};                                   \
    ::Birdy3d::ui::Size size = {};                                           \
    ::Birdy3d::ui::Placement placement = ::Birdy3d::ui::Placement::TOP_LEFT; \
    bool hidden = false;                                                     \
    std::string name = {};                                                   \
    float weight = 1.0f;                                                     \
    int column = 0;                                                          \
    int row = 0;                                                             \
    Options& merge_widget_options(::Birdy3d::ui::Widget::Options o)          \
    {                                                                        \
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

    template <class T>
    concept is_widget = std::is_base_of_v<Widget, T>;

    // Is there a better way to implement this?
    template <class T>
    concept is_specialized_widget = std::is_base_of_v<Widget, T> && !std::is_base_of_v<T, Widget>;

    template <class T>
    concept is_layout = std::is_base_of_v<Layout, T>;

    // clang-format off
    template <class T>
    concept is_widget_options = requires(T t)
    {
        { t.position } -> std::same_as<Position&>;
        { t.size } -> std::same_as<Size&>;
        { t.placement } -> std::same_as<Placement&>;
        { t.hidden } -> std::same_as<bool&>;
        { t.name } -> std::same_as<std::string&>;
        { t.weight } -> std::same_as<float&>;
        { t.column } -> std::same_as<int&>;
        { t.row } -> std::same_as<int&>;
    };
    // clang-format on

    class Widget {
    public:
        struct Options {
            BIRDY3D_WIDGET_OPTIONS_STRUCT
        };

        Widget* parent = nullptr;
        Canvas* canvas = nullptr;

        Position position = {};
        Size size = {};
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
            , row(options.row)
        { }

        virtual ~Widget() = default;

        void to_foreground(Widget* w);

        // Returns the position relative to the parent's origin in pixels
        glm::ivec2 preferred_position(glm::ivec2 parent_size, glm::ivec2 size);

        // Returns the size in pixels
        virtual glm::ivec2 minimal_size();
        glm::ivec2 preferred_size(glm::ivec2 parent_size);

        virtual void do_layout(Rect const&);
        void set_canvas(Canvas*);

        bool is_hovering();
        bool is_focused();
        bool was_last_focused();
        void focus();
        void grab_cursor();
        void ungrab_cursor();

        // Getters
        Rect absolute_rect() { return m_absolute_rect; }

        // External Event calls
        void notify_event(UIEvent&);
        void external_draw();
        bool update_hover();
        void update_visible_area(Rect const& parent_visible_area);
        virtual void late_update();
        virtual void on_update();

        // Callbacks
        typedef std::function<void(UIEvent&)> CallbackType;

        void add_callback(std::string const& name, CallbackType callback);

    protected:
        friend class Canvas;

        struct Padding {
            Dimension left;
            Dimension right;
            Dimension top;
            Dimension bottom;
        };

        std::list<std::shared_ptr<Widget>> m_children;
        std::unique_ptr<Layout> m_layout = nullptr;
        Rect m_absolute_rect{};
        Rect m_visible_area{};
        Padding m_padding;
        bool m_children_visible = true;

        virtual void draw();

        [[nodiscard]] virtual bool contains(glm::ivec2) const;

        // Callbacks
        void execute_callbacks(std::string const& name, UIEvent& event);

        bool has_callbacks(std::string const& name);

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

        // Paint
        void paint_background(bool outline) const;
        void paint_background(utils::Color const&) const;
        void paint_rectangle_filled(DimRect const&, utils::Color const& fill_color, unsigned int outline_width = 0, utils::Color const& outline_color = utils::Color::NONE) const;
        void paint_rectangle_texture(DimRect const&, render::Texture const&) const;
        void paint_triangle_filled(DimRect const&, float orientation, utils::Color const&) const;
        void paint_text(Position position, Placement placement, TextDescription const& text) const;
        void paint_rectangle_filled(Rect const&, utils::Color const& fill_color, unsigned int outline_width = 0, utils::Color const& outline_color = utils::Color::NONE) const;
        void paint_rectangle_texture(Rect const&, render::Texture const&) const;
        void paint_triangle_filled(Rect const&, float orientation, utils::Color const&) const;
        void paint_text(glm::ivec2 position, TextDescription const& text) const;

        // Layout
        template <is_layout T, typename... Args>
        void set_layout(Args... args)
        {
            m_layout = std::make_unique<T>(args...);
        }

        void unset_layout() { m_layout = nullptr; }

        void add_child(std::shared_ptr<Widget>);
        void clear_children();

        template <is_widget T>
        std::shared_ptr<T> add_child(typename T::Options options)
        {
            auto widget = std::make_shared<T>(options);
            add_child(widget);
            return std::static_pointer_cast<T>(widget);
        }

        template <is_specialized_widget T>
        std::shared_ptr<T> add_child(Options widget_options, typename T::Options specialized_options)
        {
            return add_child<T>(specialized_options.merge_widget_options(widget_options));
        }

        template <is_widget T = Widget>
        std::shared_ptr<T> get_widget(std::string const& name, bool hidden = true)
        {
            if (this->hidden && !hidden)
                return nullptr;
            for (auto const& child : m_children) {
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
