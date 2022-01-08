#pragma once

#include "ui/Widget.hpp"
#include "ui/widgets/Scrollable.hpp"
#include <string>

namespace Birdy3d::ui {

    class TextField : public Scrollable {
    public:
        bool readonly = false;
        bool multiline = false;

        TextField(Options);
        glm::vec2 minimal_size() override;
        std::string text();
        void text(std::string);
        void append(std::string);
        void clear();

    protected:
        float m_side_padding = 2;
        Text* m_text;
        bool m_selecting = false;
        bool m_changed = false;

        virtual void draw() override;

        // Events
        virtual void on_update() override;
        virtual void on_click(const events::InputClickEvent& event) override;
        virtual void on_key(const events::InputKeyEvent& event) override;
        virtual void on_char(const events::InputCharEvent& event) override;
        virtual void on_mouse_enter() override;
        virtual void on_mouse_leave() override;
        virtual void on_focus_lost() override;

        void clear_selection();
        void late_update() override;
        void scroll_if_needed(std::size_t cursor_pos);
    };

}
