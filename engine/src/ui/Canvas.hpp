#pragma once

#include "core/Application.hpp"
#include "events/EventBus.hpp"
#include "ui/AbsoluteLayout.hpp"
#include "ui/Container.hpp"

namespace Birdy3d::ui {

    class Canvas : public Container {
    public:
        bool updated = false;

        Canvas()
            : Container({ .size = 100_p }) {
            canvas = this;
            core::Application::event_bus->subscribe(this, &Canvas::on_scroll_raw);
            core::Application::event_bus->subscribe(this, &Canvas::on_click_raw);
            core::Application::event_bus->subscribe(this, &Canvas::on_key_raw);
            core::Application::event_bus->subscribe(this, &Canvas::on_char_raw);
            set_layout<AbsoluteLayout>();
        };

        void update() {
            if (!options.hidden) {
                updated = true;
                glm::vec2 viewport = core::Application::get_viewport_size();
                Widget::arrange(glm::vec2(0), viewport);
                Widget::update_visible_area(glm::vec2(0), viewport);
                if (!m_cursor_grabbed)
                    Widget::update_hover(true);
                Widget::on_update();
                Widget::late_update();
            }
        }

        void draw_canvas() {
            if (updated) {
                glClear(GL_DEPTH_BUFFER_BIT);
                glEnable(GL_SCISSOR_TEST);
                Widget::external_draw();
                glDisable(GL_SCISSOR_TEST);
            }
        }

        Widget* hovering_widget() { return m_hovering_widget; }
        Widget* focused_widget() { return m_focused_widget; }
        Widget* last_focused_widget() { return m_last_focused_widget; }
        bool cursor_grabbed() { return m_cursor_grabbed; }

        void set_focused(Widget* widget) {
            if (m_focused_widget == widget) {
                m_last_focused_widget = m_focused_widget;
                return;
            }

            if (m_focused_widget)
                m_focused_widget->on_focus_lost();
            m_last_focused_widget = m_focused_widget;
            m_focused_widget = widget;
            m_cursor_grabbed = false;
            if (widget)
                widget->on_focus();
        }

        void set_hovering(Widget* widget) {
            if (m_hovering_widget == widget)
                return;

            if (m_hovering_widget)
                m_hovering_widget->on_mouse_leave();
            m_hovering_widget = widget;
            if (widget)
                widget->on_mouse_enter();
        }

        void set_cursor_grabbed(Widget* widget, bool grabbed) {
            if (m_focused_widget != widget)
                set_focused(widget);
            m_cursor_grabbed = grabbed;
        }

        void unfocus() {
            m_focused_widget = nullptr;
            m_cursor_grabbed = false;
        }

    private:
        Widget* m_hovering_widget = nullptr;
        Widget* m_focused_widget = nullptr;
        Widget* m_last_focused_widget = nullptr;
        bool m_cursor_grabbed = false;

        void on_scroll_raw(const events::InputScrollEvent& event) {
            if (m_cursor_grabbed) {
                if (m_focused_widget && m_focused_widget != this)
                    m_focused_widget->on_scroll(event);
            } else if (m_hovering_widget && m_hovering_widget != this) {
                m_hovering_widget->on_scroll(event);
            }
        }

        void on_click_raw(const events::InputClickEvent& event) {
            if (m_cursor_grabbed) {
                if (m_focused_widget && m_focused_widget != this)
                    m_focused_widget->on_click(event);
            } else if (m_hovering_widget && m_hovering_widget != this) {
                if (event.action == GLFW_PRESS)
                    m_hovering_widget->focus();
                m_hovering_widget->on_click(event);
            }
        }

        void on_key_raw(const events::InputKeyEvent& event) {
            if (m_focused_widget && m_focused_widget != this)
                m_focused_widget->on_key(event);
        }

        void on_char_raw(const events::InputCharEvent& event) {
            if (m_focused_widget && m_focused_widget != this)
                m_focused_widget->on_char(event);
        }
    };

}
