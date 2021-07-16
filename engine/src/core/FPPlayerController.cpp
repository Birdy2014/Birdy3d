#include "core/FPPlayerController.hpp"

#include "core/Application.hpp"
#include "core/GameObject.hpp"
#include "core/Input.hpp"
#include "events/EventBus.hpp"
#include "events/InputEvents.hpp"
#include "events/WindowResizeEvent.hpp"
#include "render/Camera.hpp"
#include "ui/Canvas.hpp"
#include "ui/Layout.hpp"

namespace Birdy3d {

    FPPlayerController::FPPlayerController() { }

    void FPPlayerController::start() {
        cam = object->getComponent<Camera>();
        if (Application::canvas)
            menu = Application::canvas->getWidget<Layout>("menu");
        Application::eventBus->subscribe(this, &FPPlayerController::onResize);
        Input::setCursorHidden(true);

        if (menu) {
            menu->hidden = true;
            Application::eventBus->subscribe(this, &FPPlayerController::onKey);
        }
    }

    void FPPlayerController::cleanup() {
        Application::eventBus->unsubscribe(this, &FPPlayerController::onResize);
        if (menu) {
            Application::eventBus->unsubscribe(this, &FPPlayerController::onKey);
        }
    }

    void FPPlayerController::update() {
        // Keyboard
        if (!Input::isCursorHidden())
            return;

        float cameraSpeed = 2.5f * Application::deltaTime;
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

        float maxPitch = M_PI_2 - 0.001;
        if (this->object->transform.orientation.x > maxPitch)
            this->object->transform.orientation.x = maxPitch;
        if (this->object->transform.orientation.x < -maxPitch)
            this->object->transform.orientation.x = -maxPitch;
    }

    void FPPlayerController::onResize(WindowResizeEvent* event) {
        this->cam->resize(event->width, event->height);
    }

    void FPPlayerController::onKey(InputKeyEvent* event) {
        if (event->action != GLFW_PRESS || !(event->key == GLFW_KEY_ESCAPE || event->key == GLFW_KEY_CAPS_LOCK))
            return;
        Input::toggleCursorHidden();
        menu->hidden = !menu->hidden;
    }

}
