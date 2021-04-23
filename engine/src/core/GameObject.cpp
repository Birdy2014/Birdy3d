#include "core/GameObject.hpp"

#include "core/Component.hpp"
#include "core/RessourceManager.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d {

    GameObject::GameObject(glm::vec3 position, glm::vec3 orientation, glm::vec3 scale) {
        this->transform.position = position;
        this->transform.orientation = orientation;
        this->transform.scale = scale;
    }

    void GameObject::addChild(GameObject* c) {
        c->parent = this;
        c->transform.setParentTransform(&transform);
        this->children.push_back(c);
    }

    void GameObject::addComponent(Component* c) {
        c->object = this;
        this->components.push_back(c);
    }

    void GameObject::start() {
        for (Component* c : this->components) {
            c->start();
        }
        for (GameObject* o : this->children) {
            o->start();
        }
    }

    void GameObject::update() {
        if (hidden)
            return;

        for (Component* c : this->components) {
            c->update();
        }
        for (GameObject* o : this->children) {
            o->update();
        }
    }

    void GameObject::cleanup() {
        for (Component* c : this->components) {
            c->cleanup();
        }
    }

    glm::vec3 GameObject::absForward() {
        glm::vec3 absRot = this->transform.worldOrientation();
        glm::vec3 forward;
        forward.x = cos(absRot.y) * cos(absRot.x);
        forward.y = sin(absRot.x);
        forward.z = sin(absRot.y) * cos(absRot.x);
        return glm::normalize(forward);
    }

    glm::vec3 GameObject::absRight() {
        return glm::normalize(glm::cross(this->absForward(), glm::vec3(0, 1, 0)));
    }

    glm::vec3 GameObject::absUp() {
        return glm::normalize(glm::cross(this->absRight(), this->absForward()));
    }

    void GameObject::setScene(GameObject* scene) {
        if (scene == nullptr)
            scene = this;
        this->scene = scene;
        for (GameObject* c : this->children) {
            c->setScene(scene);
        }
    }

}
