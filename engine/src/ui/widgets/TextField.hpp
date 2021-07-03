#pragma once

#include "ui/Widget.hpp"
#include <string>

namespace Birdy3d {

    class TextField : public Widget {
    public:
        bool readonly = false;
        std::function<void()> on_change;

        TextField(UIVector position, UIVector size, Placement placement);
        std::string text();
        void text(std::string);
        void append(std::string);
        virtual void draw() override;

    protected:
        // Events
        virtual bool update(bool hover) override;
        virtual bool onClick(InputClickEvent* event, bool hover) override;
        virtual bool onKey(InputKeyEvent* event, bool hover) override;
        virtual bool onChar(InputCharEvent* event, bool hover) override;
        virtual void onMouseEnter() override;
        virtual void onMouseLeave() override;

        // Helpers
        void clear_selection();

    private:
        std::u32string m_text;
        bool m_selecting = false;
        int m_cursor_pos = -1;
        int m_selection_start = -1;
        int m_selection_end = -1;
        bool m_changed = false;

        void lateUpdate() override;
    };

}
