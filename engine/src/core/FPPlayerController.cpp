#include "core/FPPlayerController.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "events/EventBus.hpp"
#include "events/InputEvents.hpp"
#include "events/WindowResizeEvent.hpp"
#include "render/Camera.hpp"
#include "scene/GameObject.hpp"
#include "ui/Canvas.hpp"
#include "ui/Layout.hpp"

namespace Birdy3d {

    FPPlayerController::FPPlayerController() { }

    void FPPlayerController::start() {
        cam = object->get_component<Camera>();
        if (auto cam_ptr = cam.lock()) {
            glm::vec2 viewport = Application::get_viewport_size();
            cam_ptr->resize(viewport.x, viewport.y);
        }
        if (auto canvas = Application::canvas.lock())
            menu = canvas->get_widget("menu");
        Application::event_bus->subscribe(this, &FPPlayerController::onResize);
        Input::setCursorHidden(true);

        if (auto menu_ptr = menu.lock())
            menu_ptr->hidden = true;

        Application::event_bus->subscribe(this, &FPPlayerController::onKey);
    }

    void FPPlayerController::cleanup() {
        Application::event_bus->unsubscribe(this, &FPPlayerController::onResize);
        if (menu.lock()) {
            Application::event_bus->unsubscribe(this, &FPPlayerController::onKey);
        }
    }

    void FPPlayerController::update() {
        // Keyboard
        if (!Input::isCursorHidden())
            return;

        float cameraSpeed = 2.5f * Application::delta_time;
        if (Input::keyPressed(GLFW_KEY_W))
            this->object->transform.position += cameraSpeed * this->object->absForward();
        if (Input::keyPressed(GLFW_KEY_S))
            this->object->transform.position -= cameraSpeed * this->object->absForward();
        if (Input::keyPressed(GLFW_KEY_A))
            this->object->transform.position -= glm::normalize(glm::cross(this->object->absForward(), this->object->absUp())) * cameraSpeed;
        if (Input::keyPressed(GLFW_KEY_D))
            this->object->transform.position += glm::normalize(glm::cross(this->object->absForward(), this->object->absUp())) * cameraSpeed;
        if (Input::keyPressed(GLFW_KEY_SPACE))
            this->object->transform.position += glm::vec3(0.0f, cameraSpeed, 0.0f);
        if (Input::keyPressed(GLFW_KEY_LEFT_SHIFT))
            this->object->transform.position += glm::vec3(0.0f, -cameraSpeed, 0.0f);

        // Mouse
        glm::vec2 cursorOffset = Input::cursorPosOffset();
        float xoffset = cursorOffset.x;
        float yoffset = cursorOffset.y;

        float sensitivity = 0.001f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        this->object->transform.orientation.y += xoffset;
        this->object->transform.orientation.x += yoffset;

        float maxPitch = std::numbers::pi / 2 - 0.001;
        if (this->object->transform.orientation.x > maxPitch)
            this->object->transform.orientation.x = maxPitch;
        if (this->object->transform.orientation.x < -maxPitch)
            this->object->transform.orientation.x = -maxPitch;
    }

    void FPPlayerController::onResize(WindowResizeEvent* event) {
        if (auto cam_ptr = cam.lock())
            cam_ptr->resize(event->width, event->height);
    }

    void FPPlayerController::onKey(InputKeyEvent* event) {
        if (event->action != GLFW_PRESS)
            return;
        switch (event->key) {
        case GLFW_KEY_CAPS_LOCK:
        case GLFW_KEY_ESCAPE: {
            Input::toggleCursorHidden();
            if (auto menu_ptr = menu.lock())
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
