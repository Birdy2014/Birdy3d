#include "scene/GameObject.hpp"

#include "core/RessourceManager.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d {

    GameObject::GameObject(glm::vec3 position, glm::vec3 orientation, glm::vec3 scale) {
        this->transform.position = position;
        this->transform.orientation = orientation;
        this->transform.scale = scale;
    }

    void GameObject::add_child(std::unique_ptr<GameObject> c) {
        c->parent = this;
        c->transform.setParentTransform(&transform);
        c->setScene(scene);
        m_children.push_back(std::move(c));
    }

    void GameObject::add_component(std::unique_ptr<Component> c) {
        c->object = this;
        m_components.push_back(std::move(c));
    }

    void GameObject::start() {
        for (std::unique_ptr<Component>& c : m_components) {
            c->_start();
        }
        for (const std::unique_ptr<GameObject>& o : m_children) {
            o->start();
        }
    }

    void GameObject::update() {
        if (hidden)
            return;

        for (std::unique_ptr<Component>& c : m_components) {
            c->_update();
        }
        for (const std::unique_ptr<GameObject>& o : m_children) {
            o->update();
        }
    }

    void GameObject::cleanup() {
        for (std::unique_ptr<Component>& c : m_components) {
            c->_cleanup();
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

    void GameObject::setScene(Scene* scene) {
        this->scene = scene;
        for (const std::unique_ptr<GameObject>& c : m_children) {
            c->setScene(scene);
        }
    }

}
