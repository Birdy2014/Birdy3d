#pragma once

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "events/EventBus.hpp"
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
                    Widget::update_hover();
                Widget::on_update();
                Widget::late_update();
            }
        }

        void draw_canvas() {
            if (updated) {
                glDisable(GL_CULL_FACE);
                glClear(GL_DEPTH_BUFFER_BIT);
                glEnable(GL_SCISSOR_TEST);
                Widget::external_draw();
                glDisable(GL_SCISSOR_TEST);
                glEnable(GL_CULL_FACE);
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
                m_focused_widget->notify_event(Widget::EventType::FOCUS_LOST, nullptr);
            m_last_focused_widget = m_focused_widget;
            m_focused_widget = widget;
            m_cursor_grabbed = false;
            if (widget)
                widget->notify_event(Widget::EventType::FOCUS, nullptr);
        }

        void set_hovering(Widget* widget) {
            if (m_hovering_widget == widget)
                return;

            if (m_hovering_widget)
                m_hovering_widget->notify_event(Widget::EventType::MOUSE_LEAVE, nullptr);
            m_hovering_widget = widget;
            if (widget)
                widget->notify_event(Widget::EventType::MOUSE_ENTER, nullptr);
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

        void start_drag(std::any data) {
            set_focused(this);
            m_cursor_grabbed = true;
            m_dragging = true;
            m_dragging_value = data;
            core::Input::set_cursor(core::Input::CURSOR_HAND);
        }

    private:
        Widget* m_hovering_widget = nullptr;
        Widget* m_focused_widget = nullptr;
        Widget* m_last_focused_widget = nullptr;
        bool m_cursor_grabbed = false;

        bool m_dragging = false;
        std::any m_dragging_value;

        void on_scroll_raw(const events::InputScrollEvent& event) {
            if (m_cursor_grabbed) {
                if (m_focused_widget && m_focused_widget != this)
                    m_focused_widget->notify_event(Widget::EventType::SCROLL, &event);
            } else if (m_hovering_widget && m_hovering_widget != this) {
                m_hovering_widget->notify_event(Widget::EventType::SCROLL, &event);
            }
        }

        void on_click_raw(const events::InputClickEvent& event) {
            if (m_dragging) {
                if (event.action == GLFW_RELEASE) {
                    m_cursor_grabbed = false;
                    m_dragging = false;
                    Widget::update_hover();
                    m_hovering_widget->on_drop(m_dragging_value);
                    core::Input::set_cursor(core::Input::CURSOR_DEFAULT);
                }
                return;
            }

            if (m_cursor_grabbed) {
                if (m_focused_widget && m_focused_widget != this)
                    m_focused_widget->notify_event(Widget::EventType::CLICK, &event);
            } else if (m_hovering_widget && m_hovering_widget != this) {
                if (event.action == GLFW_PRESS)
                    m_hovering_widget->focus();
                m_hovering_widget->notify_event(Widget::EventType::CLICK, &event);
            }
        }

        void on_key_raw(const events::InputKeyEvent& event) {
            if (m_focused_widget && m_focused_widget != this)
                m_focused_widget->notify_event(Widget::EventType::KEY, &event);
        }

        void on_char_raw(const events::InputCharEvent& event) {
            if (m_focused_widget && m_focused_widget != this)
                m_focused_widget->notify_event(Widget::EventType::CHAR, &event);
        }
    };

}
