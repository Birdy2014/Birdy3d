#pragma once

#include "ui/Widget.hpp"
#include "ui/widgets/Scrollable.hpp"
#include <functional>
#include <string>

namespace Birdy3d::ui {

    class TextField : public Scrollable {
    public:
        bool readonly = false;
        bool multiline = false;
        std::function<void()> on_change;
        std::function<void()> on_accept;

        TextField(is_widget_options auto options)
            : Scrollable(options) {
            add_filled_rectangle(0_px, 100_pc, utils::Color::Name::BG_INPUT);
            m_text = add_text(0_px, std::string(), utils::Color::Name::FG);
        }

        glm::ivec2 minimal_size() override;
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
        virtual void on_click(ClickEvent& event) override;
        virtual void on_key(KeyEvent& event) override;
        virtual void on_char(CharEvent& event) override;
        virtual void on_mouse_enter(MouseEnterEvent&) override;
        virtual void on_mouse_leave(MouseLeaveEvent&) override;
        virtual void on_focus_lost(FocusLostEvent&) override;

        void clear_selection();
        void late_update() override;
        void scroll_if_needed(std::size_t cursor_pos);

    private:
        std::size_t char_index(glm::ivec2 pos);

        /**
         * @brief Pixel coordinates of a character in the text.
         *
         * The bottom right coordinates of the character, relative to the origin of m_text.
         */
        glm::ivec2 coordinate_of_index(std::size_t index);
    };

}
