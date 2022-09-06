#include "ui/widgets/WindowSnapArea.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "events/EventBus.hpp"
#include "ui/Canvas.hpp"
#include "ui/Theme.hpp"
#include "ui/Window.hpp"

namespace Birdy3d::ui {

    WindowSnapArea::WindowSnapArea(Options options)
        : Widget(options)
        , mode(options.mode) {
        core::Application::event_bus->subscribe(this, &WindowSnapArea::on_click_raw);
        core::Application::event_bus->subscribe(this, &WindowSnapArea::on_resize_raw);
        m_background_rect = add_filled_rectangle(0_px, 100_p, utils::Color::Name::BG);
    }

    void WindowSnapArea::on_update() {
        Widget* focused_widget = canvas->focused_widget();
        Window* focused_window = dynamic_cast<Window*>(focused_widget);
        if (!focused_window)
            return;
        if (focused_window->resizing())
            rearrange_windows();
    }

    void WindowSnapArea::on_click_raw(const events::InputClickEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_RELEASE)
            return;

        Widget* focused_widget = canvas->focused_widget();
        Window* focused_window = dynamic_cast<Window*>(focused_widget);
        if (!focused_window || !focused_window->dragged())
            return;

        if (m_background_rect->contains(core::Input::cursor_pos() - m_actual_pos) && !core::Input::key_pressed(GLFW_KEY_LEFT_SHIFT)) {
            std::vector<Window*>::iterator it;
            switch (mode) {
            case Mode::STACKING:
                m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), focused_window), m_windows.end());
                m_windows.push_back(focused_window);
                break;
            case Mode::HORIZONTAL:
                if (std::find(m_windows.begin(), m_windows.end(), focused_window) == m_windows.end()) {
                    for (it = m_windows.begin(); it != m_windows.end(); it++) {
                        if (core::Input::cursor_pos().x < (*it)->actual_pos().x + ((*it)->actual_size().x / 2))
                            break;
                    }
                    m_windows.insert(it, focused_window);
                    focused_window->size.x = m_actual_size.x / m_windows.size();
                    for (Window* window : m_windows) {
                        if (window == focused_window)
                            continue;
                        window->size.x -= focused_window->size.x / m_windows.size();
                        break;
                    }
                } else {
                    m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), focused_window), m_windows.end());
                    for (it = m_windows.begin(); it != m_windows.end(); it++) {
                        if (core::Input::cursor_pos().x < (*it)->actual_pos().x + ((*it)->actual_size().x / 2))
                            break;
                    }
                    m_windows.insert(it, focused_window);
                }
                break;
            case Mode::VERTICAL:
                if (std::find(m_windows.begin(), m_windows.end(), focused_window) == m_windows.end()) {
                    for (it = m_windows.begin(); it != m_windows.end(); it++) {
                        if (core::Input::cursor_pos().y < (*it)->actual_pos().y + ((*it)->actual_size().y / 2))
                            break;
                    }
                    m_windows.insert(it, focused_window);
                    focused_window->size.y = m_actual_size.y / m_windows.size();
                    for (Window* window : m_windows) {
                        if (window == focused_window)
                            continue;
                        window->size.y -= focused_window->size.y / m_windows.size();
                        break;
                    }
                } else {
                    m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), focused_window), m_windows.end());
                    for (it = m_windows.begin(); it != m_windows.end(); it++) {
                        if (core::Input::cursor_pos().y < (*it)->actual_pos().y + ((*it)->actual_size().y / 2))
                            break;
                    }
                    m_windows.insert(it, focused_window);
                }
                break;
            }
        } else {
            // Remove window (if exists)
            m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), focused_window), m_windows.end());
        }

        rearrange_windows();
    }

    void WindowSnapArea::on_resize_raw(const events::WindowResizeEvent&) {
        rearrange_windows();
    }

    void WindowSnapArea::rearrange_windows() {
        glm::vec2 new_size = m_actual_size;
        glm::vec2 new_pos = m_actual_pos;
        glm::vec2 size_sum = glm::vec2(0);
        float size_diff;

        if (mode != Mode::STACKING) {
            for (Window* window : m_windows)
                size_sum += glm::max(window->minimal_size(), window->size.to_pixels());
        }

        Widget* focused_widget = canvas->focused_widget();
        Window* focused_window = dynamic_cast<Window*>(focused_widget);
        if (focused_window && std::find(m_windows.begin(), m_windows.end(), focused_window) == m_windows.end())
            focused_window = nullptr;

        for (size_t i = 0; i < m_windows.size(); i++) {
            Window* window = m_windows[i];
            switch (mode) {
            case Mode::STACKING:
                window->position = m_actual_pos;
                window->size = new_size;
                new_size.y -= core::Application::theme().line_height();
                break;
            case Mode::HORIZONTAL: {
                if (focused_window && (focused_window->resizing_left() || focused_window->resizing_right())) {
                    size_diff = 0;
                    if (window == focused_window) {
                        if (focused_window->resizing_left()) {
                            if (i == 0) {
                                focused_window->size.x += m_actual_size.x - size_sum.x;
                                focused_window->position = m_actual_pos;
                            } else {
                                m_windows[i - 1]->size.x += m_actual_size.x - size_sum.x;
                                new_pos.x += m_actual_size.x - size_sum.x;
                            }
                        } else {
                            if (i == m_windows.size() - 1)
                                focused_window->size.x += m_actual_size.x - size_sum.x;
                            else
                                m_windows[i + 1]->size.x += m_actual_size.x - size_sum.x;
                        }
                    }
                } else {
                    size_diff = (m_actual_size.x - size_sum.x) / m_windows.size();
                }
                window->position = new_pos;
                window->size.y = m_actual_size.y;
                window->size.x += size_diff;
                new_pos.x += std::max(window->minimal_size().x, window->size.x.to_pixels());
                break;
            }
            case Mode::VERTICAL:
                if (focused_window && (focused_window->resizing_top() || focused_window->resizing_bottom())) {
                    size_diff = 0;
                    if (window == focused_window) {
                        if (focused_window->resizing_top()) {
                            if (i == m_windows.size() - 1) {
                                focused_window->position = m_actual_pos;
                            } else {
                                m_windows[i + 1]->size.y += m_actual_size.y - size_sum.y;
                            }
                        } else {
                            if (i == 0) {
                                focused_window->size.y += m_actual_size.y - size_sum.y;
                                focused_window->position = m_actual_pos;
                            } else {
                                m_windows[i - 1]->size.y += m_actual_size.y - size_sum.y;
                                new_pos.y += m_actual_size.y - size_sum.y;
                            }
                        }
                    }
                } else {
                    size_diff = (m_actual_size.y - size_sum.y) / m_windows.size();
                }
                window->position = new_pos;
                window->size.x = m_actual_size.x;
                window->size.y += size_diff;
                new_pos.y += std::max(window->minimal_size().y, window->size.y.to_pixels());
                break;
            }
        }
    }

}
