#include "core/FPPlayerController.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "events/EventBus.hpp"
#include "events/InputEvents.hpp"
#include "events/WindowResizeEvent.hpp"
#include "render/Camera.hpp"
#include "scene/Entity.hpp"
#include "ui/Canvas.hpp"
#include "ui/Layout.hpp"

namespace Birdy3d {

    FPPlayerController::FPPlayerController() { }

    void FPPlayerController::start() {
        m_cam = entity->get_component<Camera>();
        if (auto cam_ptr = m_cam.lock()) {
            glm::vec2 viewport = Application::get_viewport_size();
            cam_ptr->resize(viewport.x, viewport.y);
        }
        if (auto canvas = Application::canvas.lock())
            m_menu = canvas->get_widget("menu");
        Application::event_bus->subscribe(this, &FPPlayerController::on_resize);
        Input::set_cursor_hidden(true);

        if (auto menu_ptr = m_menu.lock())
            menu_ptr->hidden = true;

        Application::event_bus->subscribe(this, &FPPlayerController::on_key);
    }

    void FPPlayerController::cleanup() {
        Application::event_bus->unsubscribe(this, &FPPlayerController::on_resize);
        if (m_menu.lock()) {
            Application::event_bus->unsubscribe(this, &FPPlayerController::on_key);
        }
    }

    void FPPlayerController::update() {
        // Keyboard
        if (!Input::is_cursor_hidden())
            return;

        float camera_speed = 2.5f * Application::delta_time;
        if (Input::key_pressed(GLFW_KEY_W))
            entity->transform.position += camera_speed * entity->world_forward();
        if (Input::key_pressed(GLFW_KEY_S))
            entity->transform.position -= camera_speed * entity->world_forward();
        if (Input::key_pressed(GLFW_KEY_A))
            entity->transform.position -= glm::normalize(glm::cross(entity->world_forward(), entity->world_up())) * camera_speed;
        if (Input::key_pressed(GLFW_KEY_D))
            entity->transform.position += glm::normalize(glm::cross(entity->world_forward(), entity->world_up())) * camera_speed;
        if (Input::key_pressed(GLFW_KEY_SPACE))
            entity->transform.position += glm::vec3(0.0f, camera_speed, 0.0f);
        if (Input::key_pressed(GLFW_KEY_LEFT_SHIFT))
            entity->transform.position += glm::vec3(0.0f, -camera_speed, 0.0f);

        // Mouse
        glm::vec2 cursor_offset = Input::cursor_pos_offset();
        float xoffset = cursor_offset.x;
        float yoffset = cursor_offset.y;

        float sensitivity = 0.001f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        entity->transform.orientation.y += xoffset;
        entity->transform.orientation.x += yoffset;

        float max_pitch = std::numbers::pi / 2 - 0.001;
        if (entity->transform.orientation.x > max_pitch)
            entity->transform.orientation.x = max_pitch;
        if (entity->transform.orientation.x < -max_pitch)
            entity->transform.orientation.x = -max_pitch;
    }

    void FPPlayerController::on_resize(WindowResizeEvent* event) {
        if (auto cam_ptr = m_cam.lock())
            cam_ptr->resize(event->width, event->height);
    }

    void FPPlayerController::on_key(InputKeyEvent* event) {
        if (event->action != GLFW_PRESS)
            return;
        switch (event->key) {
        case GLFW_KEY_CAPS_LOCK:
        case GLFW_KEY_ESCAPE: {
            Input::toggle_cursor_hidden();
            if (auto menu_ptr = m_menu.lock())
                menu_ptr->hidden = !menu_ptr->hidden;
            break;
        }
        case GLFW_KEY_F: {
            if (auto flashlight_ptr = flashlight.lock())
                flashlight_ptr->hidden = !flashlight_ptr->hidden;
            break;
        }
        }
    }

}
