#include "utils/FPPlayerController.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "ecs/Entity.hpp"
#include "events/EventBus.hpp"
#include "events/InputEvents.hpp"
#include "render/Camera.hpp"
#include "ui/Canvas.hpp"

namespace Birdy3d::utils {

    FPPlayerController::FPPlayerController() { }

    void FPPlayerController::start() {
        m_cam = entity->get_component<render::Camera>();
        if (auto canvas = core::Application::canvas.lock())
            m_menu = canvas->get_widget("menu");
        core::Input::set_cursor_hidden(true);

        if (auto menu_ptr = m_menu.lock())
            menu_ptr->hidden = true;

        core::Application::event_bus->subscribe(this, &FPPlayerController::on_key);
    }

    void FPPlayerController::cleanup() {
        if (m_menu.lock()) {
            core::Application::event_bus->unsubscribe(this, &FPPlayerController::on_key);
        }
    }

    void FPPlayerController::update() {
        // Keyboard
        if (!core::Input::is_cursor_hidden())
            return;

        float camera_speed = 2.5f * core::Application::delta_time;
        if (core::Input::key_pressed(GLFW_KEY_W))
            entity->transform.position += camera_speed * entity->world_forward();
        if (core::Input::key_pressed(GLFW_KEY_S))
            entity->transform.position -= camera_speed * entity->world_forward();
        if (core::Input::key_pressed(GLFW_KEY_A))
            entity->transform.position -= glm::normalize(glm::cross(entity->world_forward(), entity->world_up())) * camera_speed;
        if (core::Input::key_pressed(GLFW_KEY_D))
            entity->transform.position += glm::normalize(glm::cross(entity->world_forward(), entity->world_up())) * camera_speed;
        if (core::Input::key_pressed(GLFW_KEY_SPACE))
            entity->transform.position += glm::vec3(0.0f, camera_speed, 0.0f);
        if (core::Input::key_pressed(GLFW_KEY_LEFT_SHIFT))
            entity->transform.position += glm::vec3(0.0f, -camera_speed, 0.0f);

        // Mouse
        glm::vec2 cursor_offset = core::Input::cursor_pos_offset();
        float xoffset = cursor_offset.x;
        float yoffset = cursor_offset.y;

        float sensitivity = 0.001f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        entity->transform.orientation.y += xoffset;
        entity->transform.orientation.x -= yoffset;

        float max_pitch = std::numbers::pi / 2 - 0.001;
        if (entity->transform.orientation.x > max_pitch)
            entity->transform.orientation.x = max_pitch;
        if (entity->transform.orientation.x < -max_pitch)
            entity->transform.orientation.x = -max_pitch;
    }

    void FPPlayerController::on_key(const events::InputKeyEvent& event) {
        if (event.action != GLFW_PRESS)
            return;
        switch (event.key) {
        case GLFW_KEY_CAPS_LOCK:
        case GLFW_KEY_ESCAPE: {
            core::Input::toggle_cursor_hidden();
            if (auto menu_ptr = m_menu.lock()) {
                menu_ptr->hidden = !menu_ptr->hidden;
                if (menu_ptr->hidden)
                    menu_ptr->canvas->unfocus();
            }
            break;
        }
        case GLFW_KEY_F: {
            if (auto flashlight_ptr = flashlight.lock())
                flashlight_ptr->hidden = !flashlight_ptr->hidden;
            break;
        }
        }
    }

    void FPPlayerController::serialize(serializer::Adapter& adapter) {
        adapter("flashlight", flashlight);
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, FPPlayerController);

}
