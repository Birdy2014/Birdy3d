#include "FPPlayerController.hpp"
#include "Input.hpp"
#include "Engine.hpp"
#include "GameObject.hpp"
#include "../render/Camera.hpp"

FPPlayerController::FPPlayerController() {

}

void FPPlayerController::start() {
	this->cam = this->object->getComponent<Camera>();
	Engine::getFramebufferSizeEventHandler()->addHandler([&](framebufferSizeArg arg) {
		this->cam->resize(arg.width, arg.height);
	});
	Input::setCursorHidden(true);
}

void FPPlayerController::update(float deltaTime) {
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

	float cameraSpeed = 2.5f * deltaTime;
	if (Input::keyPressed(GLFW_KEY_W))
        this->object->pos += cameraSpeed * this->object->absForward();
	if (Input::keyPressed(GLFW_KEY_S))
        this->object->pos -= cameraSpeed * this->object->absForward();
	if (Input::keyPressed(GLFW_KEY_A))
        this->object->pos -= glm::normalize(glm::cross(this->object->absForward(), this->object->absUp())) * cameraSpeed;
	if (Input::keyPressed(GLFW_KEY_D))
        this->object->pos += glm::normalize(glm::cross(this->object->absForward(), this->object->absUp())) * cameraSpeed;
	if (Input::keyPressed(GLFW_KEY_SPACE))
		this->object->pos += glm::vec3(0.0f, cameraSpeed, 0.0f);
	if (Input::keyPressed(GLFW_KEY_LEFT_SHIFT))
		this->object->pos += glm::vec3(0.0f, -cameraSpeed, 0.0f);

	// Mouse
	glm::vec2 cursorOffset = Input::cursorPosOffset();
	float xoffset = cursorOffset.x;
	float yoffset = cursorOffset.y;

	float sensitivity = 0.001f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	this->object->rot.y += xoffset;
	this->object->rot.x -= yoffset;

	float maxPitch = M_PI_2 - 0.001;
	if(this->object->rot.x > maxPitch)
  		this->object->rot.x =  maxPitch;
	if(this->object->rot.x < -maxPitch)
  		this->object->rot.x = -maxPitch;
}
