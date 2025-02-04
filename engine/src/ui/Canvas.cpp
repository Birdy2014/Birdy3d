#include "ui/Canvas.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "events/EventBus.hpp"
#include "events/InputEvents.hpp"

namespace Birdy3d::ui {

    Canvas::Canvas()
        : Container(Widget::Options{.size = 100_pc})
    {
        canvas = this;
        core::Application::event_bus->subscribe(this, &Canvas::on_scroll_raw);
        core::Application::event_bus->subscribe(this, &Canvas::on_click_raw);
        core::Application::event_bus->subscribe(this, &Canvas::on_key_raw);
        core::Application::event_bus->subscribe(this, &Canvas::on_char_raw);
        set_layout<AbsoluteLayout>();
    };

    void Canvas::update()
    {
        if (!hidden) {
            updated = true;
            auto viewport_size = core::Application::get_viewport_size();
            auto viewport_rect = Rect::from_position_and_size(glm::ivec2(0), viewport_size);

            if (!m_cursor_grabbed)
                Widget::update_hover();
            Widget::on_update();

            // Do layout after update to allow positions and sizes to be changed in `on_update`
            Widget::do_layout(viewport_rect);
            Widget::update_visible_area(viewport_rect);

            Widget::late_update();
        }
    }

    void Canvas::draw_canvas()
    {
        if (updated) {
            glDisable(GL_CULL_FACE);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_SCISSOR_TEST);
            Widget::external_draw();
            glDisable(GL_SCISSOR_TEST);
            glEnable(GL_CULL_FACE);
        }
    }

    void Canvas::set_focused(Widget* widget)
    {
        if (m_focused_widget == widget) {
            m_last_focused_widget = m_focused_widget;
            return;
        }

        if (m_focused_widget) {
            auto ui_event = FocusLostEvent{};
            m_focused_widget->notify_event(ui_event);
        }
        m_last_focused_widget = m_focused_widget;
        m_focused_widget = widget;
        m_cursor_grabbed = false;
        if (widget) {
            auto ui_event = FocusEvent{};
            widget->notify_event(ui_event);
        }
    }

    void Canvas::set_hovering(Widget* widget)
    {
        if (m_hovering_widget == widget)
            return;

        if (m_hovering_widget) {
            auto ui_event = MouseLeaveEvent{};
            m_hovering_widget->notify_event(ui_event);
        }
        m_hovering_widget = widget;
        if (widget) {
            auto ui_event = MouseEnterEvent{};
            widget->notify_event(ui_event);
        }
    }

    void Canvas::set_cursor_grabbed(Widget* widget, bool grabbed)
    {
        if (m_focused_widget != widget)
            set_focused(widget);
        m_cursor_grabbed = grabbed;
    }

    void Canvas::unfocus()
    {
        m_focused_widget = nullptr;
        m_cursor_grabbed = false;
    }

    void Canvas::start_drag(std::any data)
    {
        set_focused(this);
        m_cursor_grabbed = true;
        m_dragging = true;
        m_dragging_value = data;
        core::Input::set_cursor(core::Input::CURSOR_HAND);
    }

    void Canvas::on_scroll_raw(events::InputScrollEvent const& event)
    {
        if (m_cursor_grabbed) {
            if (m_focused_widget && m_focused_widget != this) {
                auto ui_event = ScrollEvent{event.xoffset, event.yoffset};
                m_focused_widget->notify_event(ui_event);
            }
        } else if (m_hovering_widget && m_hovering_widget != this) {
            auto ui_event = ScrollEvent{event.xoffset, event.yoffset};
            m_hovering_widget->notify_event(ui_event);
        }
    }

    void Canvas::on_click_raw(events::InputClickEvent const& event)
    {
        if (m_dragging) {
            if (event.action == GLFW_RELEASE) {
                m_cursor_grabbed = false;
                m_dragging = false;
                Widget::update_hover();

                auto drop_event = DropEvent{m_dragging_value};
                m_hovering_widget->notify_event(drop_event);

                core::Input::set_cursor(core::Input::CURSOR_DEFAULT);
            }
            return;
        }

        if (m_cursor_grabbed) {
            if (m_focused_widget && m_focused_widget != this) {
                auto ui_event = ClickEvent{event.button, event.action, event.mods};
                m_focused_widget->notify_event(ui_event);
            }
            return;
        }

        if (m_hovering_widget && m_hovering_widget != this) {
            if (!m_hovering_widget->is_focused() && event.action == GLFW_PRESS)
                m_hovering_widget->focus();

            if (m_hovering_widget->is_focused()) {
                auto ui_event = ClickEvent{event.button, event.action, event.mods};
                m_hovering_widget->notify_event(ui_event);
            }
        }
    }

    void Canvas::on_key_raw(events::InputKeyEvent const& event)
    {
        if (m_focused_widget && m_focused_widget != this) {
            auto ui_event = KeyEvent{event.key, event.scancode, event.action, event.mods};
            m_focused_widget->notify_event(ui_event);
        }
    }

    void Canvas::on_char_raw(events::InputCharEvent const& event)
    {
        if (m_focused_widget && m_focused_widget != this) {
            auto ui_event = CharEvent{event.codepoint};
            m_focused_widget->notify_event(ui_event);
        }
    }
}
