#pragma once

#include <any>

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

        explicit UIEvent(Type type)
            : type { type } { }

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
    };

    class CharEvent : public UIEvent {
    public:
        const unsigned int codepoint;

        CharEvent(unsigned int codepoint)
            : UIEvent(UIEvent::CHAR)
            , codepoint(codepoint) { }
    };

    class MouseEnterEvent : public UIEvent {
    public:
        MouseEnterEvent()
            : UIEvent(UIEvent::MOUSE_ENTER) { }
    };

    class MouseLeaveEvent : public UIEvent {
    public:
        MouseLeaveEvent()
            : UIEvent(UIEvent::MOUSE_LEAVE) { }
    };

    class FocusEvent : public UIEvent {
    public:
        FocusEvent()
            : UIEvent(UIEvent::FOCUS) { }
    };

    class FocusLostEvent : public UIEvent {
    public:
        FocusLostEvent()
            : UIEvent(UIEvent::FOCUS_LOST) { }
    };

    class ResizeEvent : public UIEvent {
    public:
        ResizeEvent()
            : UIEvent(UIEvent::RESIZE) { }
    };

    class DropEvent : public UIEvent {
    public:
        const std::any data;

        DropEvent(std::any data)
            : UIEvent(UIEvent::DROP)
            , data(data) { }
    };

}
