#include "ui/widgets/WindowSnapArea.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "events/EventBus.hpp"
#include "ui/Canvas.hpp"
#include "ui/Theme.hpp"
#include "ui/Window.hpp"

namespace Birdy3d {

    WindowSnapArea::WindowSnapArea(UIVector pos, UIVector size, Placement placement, Mode mode)
        : Widget(pos, size, placement)
        , mode(mode) {
        Application::event_bus->subscribe(this, &WindowSnapArea::on_click_raw);
        Application::event_bus->subscribe(this, &WindowSnapArea::on_resize_raw);
        m_background_rect = add_filled_rectangle(0_px, 100_p, Application::theme->color_bg);
    }

    void WindowSnapArea::on_click_raw(InputClickEvent* event) {
        if (event->button != GLFW_MOUSE_BUTTON_LEFT || event->action != GLFW_RELEASE)
            return;

        Widget* focused_widget = canvas->focused_widget();
        Window* focused_window = dynamic_cast<Window*>(focused_widget);
        if (!focused_window)
            return;

        m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), focused_window), m_windows.end());

        if (m_background_rect->contains(Input::cursorPos() - m_actual_pos) && !Input::keyPressed(GLFW_KEY_LEFT_SHIFT)) {
            if (std::find(m_windows.begin(), m_windows.end(), focused_window) == m_windows.end())
                m_windows.push_back(focused_window);
        }

        recalculate_sizes();

        // TODO: Put windows over each other
    }

    void WindowSnapArea::on_resize_raw(WindowResizeEvent* event) {
        recalculate_sizes();
    }

    void WindowSnapArea::recalculate_sizes() {
        glm::vec2 new_size = m_actual_size;
        for (Window* window : m_windows) {
            window->pos = m_actual_pos;
            window->size = new_size;

            new_size.y -= theme->line_height;
        }
    }

}
