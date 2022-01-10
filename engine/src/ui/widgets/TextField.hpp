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
        void scroll_down();

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

    private:
        std::size_t char_index(glm::vec2 pos);

        /**
         * @brief Pixel coordinates of a character in the text.
         *
         * The bottom right coordinates of the character, relative to the origin of m_text.
         */
        glm::vec2 coordinate_of_index(std::size_t index);
    };

}
