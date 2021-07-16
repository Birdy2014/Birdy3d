#pragma once

#include "core/Application.hpp"
#include "events/EventBus.hpp"
#include "ui/Widget.hpp"
#include "ui/widgets/AbsoluteLayout.hpp"

namespace Birdy3d {

    class Canvas : public AbsoluteLayout {
    public:
        bool updated = false;

        Canvas()
            : AbsoluteLayout(0_px, 100_p) {
            canvas = this;
            Application::eventBus->subscribe(this, &Canvas::on_scroll);
            Application::eventBus->subscribe(this, &Canvas::on_click);
            Application::eventBus->subscribe(this, &Canvas::on_key);
            Application::eventBus->subscribe(this, &Canvas::on_char);
        };

        void update() {
            if (!hidden) {
                updated = true;
                glm::vec2 viewport = Application::getViewportSize();
                AbsoluteLayout::arrange(glm::vec2(0), viewport);
                if (!m_cursor_grabbed)
                    AbsoluteLayout::update_hover(true);
                AbsoluteLayout::on_update();
                AbsoluteLayout::late_update();
            }
        }

        void draw() override {
            if (updated) {
                glClear(GL_DEPTH_BUFFER_BIT);
                glEnable(GL_SCISSOR_TEST);
                AbsoluteLayout::draw();
                glDisable(GL_SCISSOR_TEST);
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
