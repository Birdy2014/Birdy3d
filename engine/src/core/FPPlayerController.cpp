#include "core/FPPlayerController.hpp"

#include "core/Application.hpp"
#include "core/GameObject.hpp"
#include "core/Input.hpp"
#include "events/WindowResizeEvent.hpp"
#include "render/Camera.hpp"
#include "ui/Canvas.hpp"

namespace Birdy3d {

    FPPlayerController::FPPlayerController() {
    }

    void FPPlayerController::start() {
        this->cam = this->object->getComponent<Camera>();
        Application::eventBus->subscribe(this, &FPPlayerController::onResize);
        Input::setCursorHidden(true);
    }

    void FPPlayerController::update() {
        // Keyboard
        if (Input::keyPressed(GLFW_KEY_ESCAPE)) {
            if (!this->hiddenStatusUpdated) {
                hiddenStatusUpdated = true;
                Input::toggleCursorHidden();
                this->cam->canvas->hidden = !this->cam->canvas->hidden;
            }
        } else {
            this->hiddenStatusUpdated = false;
        }

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

}
