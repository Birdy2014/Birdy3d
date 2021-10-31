#pragma once

#include "core/Application.hpp"
#include "events/EventBus.hpp"
#include "ui/AbsoluteLayout.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d {

    class Canvas : public Widget {
    public:
        bool updated = false;

        Canvas()
            : Widget(0_px, 100_p) {
            canvas = this;
            Application::event_bus->subscribe(this, &Canvas::on_scroll_raw);
            Application::event_bus->subscribe(this, &Canvas::on_click_raw);
            Application::event_bus->subscribe(this, &Canvas::on_key_raw);
            Application::event_bus->subscribe(this, &Canvas::on_char_raw);
            set_layout<AbsoluteLayout>();
        };

        void update() {
            if (!hidden) {
                updated = true;
                glm::vec2 viewport = Application::get_viewport_size();
                Widget::arrange(glm::vec2(0), viewport);
                if (!m_cursor_grabbed)
                    Widget::update_hover(true);
                Widget::on_update();
                Widget::late_update();
            }
        }

        void draw() override {
            if (updated) {
                glClear(GL_DEPTH_BUFFER_BIT);
                Widget::draw();
            }
        }

        Widget* hovering_widget() { return m_hovering_widget; }
        Widget* focused_widget() { return m_focused_widget; }
        Widget* last_focused_widget() { return m_last_focused_widget; }
        bool cursor_grabbed() { return m_cursor_grabbed; }

    private:
        friend class Widget;

        Widget* m_hovering_widget = nullptr;
        Widget* m_focused_widget = nullptr;
        Widget* m_last_focused_widget = nullptr;
        bool m_cursor_grabbed = false;

        void on_scroll_raw(const InputScrollEvent& event) {
            if (m_cursor_grabbed) {
                if (m_focused_widget && m_focused_widget != this)
                    m_focused_widget->on_scroll(event);
            } else if (m_hovering_widget && m_hovering_widget != this) {
                m_hovering_widget->on_scroll(event);
            }
        }

        void on_click_raw(const InputClickEvent& event) {
            if (m_cursor_grabbed) {
                if (m_focused_widget && m_focused_widget != this)
                    m_focused_widget->on_click(event);
            } else if (m_hovering_widget && m_hovering_widget != this) {
                if (event.action == GLFW_PRESS)
                    m_hovering_widget->focus();
                m_hovering_widget->on_click(event);
            }
        }

        void on_key_raw(const InputKeyEvent& event) {
            if (m_focused_widget && m_focused_widget != this)
                m_focused_widget->on_key(event);
        }

        void on_char_raw(const InputCharEvent& event) {
            if (m_focused_widget && m_focused_widget != this)
                m_focused_widget->on_char(event);
        }
    };

}
