#include "Camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
    this->pos = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(pos, pos + front, up);
}

void Camera::updateCameraVectors() {
    // Calculate the new Front vector
    this->front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front.y = sin(glm::radians(pitch));
    this->front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);
    // Calculate the Right and Up vector
    this->right = glm::normalize(glm::cross(front, worldUp));
    this->up    = glm::normalize(glm::cross(right, front));
}