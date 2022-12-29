#pragma once

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
            : Scrollable(options)
        {
            m_text = std::make_unique<Text>(0_px, std::string(), utils::Color::Name::FG, Placement::TOP_LEFT);
        }

        glm::ivec2 minimal_size() override;
        std::string text();
        void text(std::string);
        void append(std::string);
        void clear();
        void scroll_down();

    protected:
        float m_side_padding = 2;
        std::unique_ptr<Text> m_text;
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
        virtual glm::ivec2 content_size() override;

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
