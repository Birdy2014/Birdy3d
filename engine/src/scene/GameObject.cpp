#include "scene/GameObject.hpp"

#include "core/RessourceManager.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d {

    GameObject::GameObject(std::string name, glm::vec3 position, glm::vec3 orientation, glm::vec3 scale)
        : name(name) {
        transform.position = position;
        transform.orientation = orientation;
        transform.scale = scale;
    }

    void GameObject::add_child(std::shared_ptr<GameObject> c) {
        c->parent = this;
        c->setScene(scene);
        m_children.push_back(std::move(c));
    }

    void GameObject::add_component(std::shared_ptr<Component> c) {
        c->object = this;
        m_components.push_back(std::move(c));
    }

    void GameObject::remove_child(GameObject* to_remove) {
        if (std::find_if(m_children.begin(), m_children.end(), [&](std::shared_ptr<GameObject> object) { return object.get() == to_remove; }) != m_children.end())
            to_remove->cleanup();
        m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [&](std::shared_ptr<GameObject> child) { return child.get() == to_remove; }), m_children.end());
    }

    void GameObject::remove_component(Component* to_remove) {
        if (std::find_if(m_components.begin(), m_components.end(), [&](std::shared_ptr<Component> component) { return component.get() == to_remove; }) != m_components.end())
            to_remove->_cleanup();
        m_components.erase(std::remove_if(m_components.begin(), m_components.end(), [&](std::shared_ptr<Component> component) { return component.get() == to_remove; }), m_components.end());
    }

    void GameObject::remove() {
        if (parent)
            parent->remove_child(this);
    }

    void GameObject::start() {
        for (const auto& c : m_components) {
            c->_start();
        }
        for (const auto& o : m_children) {
            o->start();
        }
    }

    void GameObject::update() {
        if (hidden)
            return;

        for (const auto& c : m_components) {
            c->_update();
        }
        for (const auto& o : m_children) {
            o->update();
        }
    }

    void GameObject::post_update() {
        if (hidden)
            return;

        transform.post_update();

        for (const auto& o : m_children) {
            o->post_update();
        }
    }

    void GameObject::cleanup() {
        for (const auto& c : m_components) {
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
        for (const auto& c : m_children) {
            c->setScene(scene);
        }
    }

}
