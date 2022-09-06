#pragma once

#include <any>
#include <fmt/format.h>
#include <type_traits>

namespace Birdy3d::ui {

    class UIEvent {
    public:
        enum Type {
            SCROLL,
            CLICK,
            KEY,
            CHAR,
            MOUSE_ENTER,
            MOUSE_LEAVE,
            FOCUS,
            FOCUS_LOST,
            RESIZE,
            DROP,
        };

        Type type;

        virtual ~UIEvent() = default;

        void handled() {
            m_bubbles = false;
        }

        bool bubbles() {
            switch (type) {
            case SCROLL:
            case CLICK:
            case KEY:
            case CHAR:
                return m_bubbles;
            default:
                return false;
            }
        }

        virtual std::string debug_text() const { return "UIEvent ()"; }

    protected:
        explicit UIEvent(Type type)
            : type { type } { }

    private:
        bool m_bubbles { true };
    };

    class ScrollEvent : public UIEvent {
    public:
        const double xoffset;
        const double yoffset;

        ScrollEvent(double xoffset, double yoffset)
            : UIEvent(UIEvent::SCROLL)
            , xoffset(xoffset)
            , yoffset(yoffset) { }

        virtual std::string debug_text() const override {
            return fmt::format("ScrollEvent ( xoffset: {}, yoffset: {} )", xoffset, yoffset);
        }
    };

    class ClickEvent : public UIEvent {
    public:
        const int button;
        const int action;
        const int mods;

        ClickEvent(int button, int action, int mods)
            : UIEvent(UIEvent::CLICK)
            , button(button)
            , action(action)
            , mods(mods) { }

        virtual std::string debug_text() const override {
            // FIXME: Make format prettier
            return fmt::format("ClickEvent ( button: {}, action: {}, mods: {} )", button, action, mods);
        }
    };

    class KeyEvent : public UIEvent {
    public:
        const int key;
        const int scancode;
        const int action;
        const int mods;

        KeyEvent(int key, int scancode, int action, int mods)
            : UIEvent(UIEvent::KEY)
            , key(key)
            , scancode(scancode)
            , action(action)
            , mods(mods) { }

        virtual std::string debug_text() const override {
            // FIXME: Make format prettier
            return fmt::format("KeyEvent ( key: {}, action: {}, mods: {} )", key, action, mods);
        }
    };

    class CharEvent : public UIEvent {
    public:
        const unsigned int codepoint;

        CharEvent(unsigned int codepoint)
            : UIEvent(UIEvent::CHAR)
            , codepoint(codepoint) { }

        virtual std::string debug_text() const override {
            return fmt::format("CharEvent ( codepoint: {} )", codepoint);
        }
    };

    class MouseEnterEvent : public UIEvent {
    public:
        MouseEnterEvent()
            : UIEvent(UIEvent::MOUSE_ENTER) { }

        virtual std::string debug_text() const override {
            return fmt::format("MouseEnterEvent ()");
        }
    };

    class MouseLeaveEvent : public UIEvent {
    public:
        MouseLeaveEvent()
            : UIEvent(UIEvent::MOUSE_LEAVE) { }

        virtual std::string debug_text() const override {
            return fmt::format("MouseLeaveEvent ()");
        }
    };

    class FocusEvent : public UIEvent {
    public:
        FocusEvent()
            : UIEvent(UIEvent::FOCUS) { }

        virtual std::string debug_text() const override {
            return fmt::format("FocusEvent ()");
        }
    };

    class FocusLostEvent : public UIEvent {
    public:
        FocusLostEvent()
            : UIEvent(UIEvent::FOCUS_LOST) { }

        virtual std::string debug_text() const override {
            return fmt::format("FocusLostEvent ()");
        }
    };

    class ResizeEvent : public UIEvent {
    public:
        ResizeEvent()
            : UIEvent(UIEvent::RESIZE) { }

        virtual std::string debug_text() const override {
            return fmt::format("ResizeEvent ()");
        }
    };

    class DropEvent : public UIEvent {
    public:
        const std::any data;

        DropEvent(std::any data)
            : UIEvent(UIEvent::DROP)
            , data(data) { }

        virtual std::string debug_text() const override {
            return fmt::format("DropEvent ( datatype: {} )", data.type().name());
        }
    };

    template <class EventType>
    concept is_event = std::is_base_of_v<UIEvent, EventType>;

}

template <Birdy3d::ui::is_event EventType>
struct fmt::formatter<EventType> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}')
            throw format_error("invalid format");
        return it;
    }

    using Type = Birdy3d::ui::UIEvent::Type;

    template <typename FormatContext>
    auto format(const EventType& event, FormatContext& ctx) -> decltype(ctx.out()) {
        return format_to(ctx.out(), "{}", event.debug_text());
    }
};
