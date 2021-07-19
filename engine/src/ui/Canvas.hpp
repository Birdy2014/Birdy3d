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
            Application::eventBus->subscribe(this, &Canvas::on_scroll);
            Application::eventBus->subscribe(this, &Canvas::on_click);
            Application::eventBus->subscribe(this, &Canvas::on_key);
            Application::eventBus->subscribe(this, &Canvas::on_char);
            set_layout<AbsoluteLayout>();
        };

        void update() {
            if (!hidden) {
                updated = true;
                glm::vec2 viewport = Application::getViewportSize();
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

    private:
        friend class Widget;

        Widget* m_hovering_widget = nullptr;
        Widget* m_focused_widget = nullptr;
        bool m_cursor_grabbed = false;

        void on_scroll(InputScrollEvent* event) {
            if (m_cursor_grabbed) {
                if (m_focused_widget && m_focused_widget != this)
                    m_focused_widget->on_scroll(event);
            } else if (m_hovering_widget && m_hovering_widget != this) {
                m_hovering_widget->on_scroll(event);
            }
        }

        void on_click(InputClickEvent* event) {
            if (m_cursor_grabbed) {
                if (m_focused_widget && m_focused_widget != this)
                    m_focused_widget->on_click(event);
            } else if (m_hovering_widget && m_hovering_widget != this) {
                if (m_hovering_widget != m_focused_widget && event->action == GLFW_PRESS)
                    m_hovering_widget->focus();
                m_hovering_widget->on_click(event);
            }
        }

        void on_key(InputKeyEvent* event) {
            if (m_focused_widget && m_focused_widget != this)
                m_focused_widget->on_key(event);
        }

        void on_char(InputCharEvent* event) {
            if (m_focused_widget && m_focused_widget != this)
                m_focused_widget->on_char(event);
        }
    };

}
