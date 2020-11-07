#include "GameObject.hpp"

GameObject::GameObject(Shader *s, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) {
    this->shader = s;
    this->pos = pos;
    this->rot = rot;
    this->scale = scale;
}

void GameObject::addChild(GameObject *c) {
    c->parent = this;
    this->children.push_back(c);
}

void GameObject::addComponent(Component *c) {
    c->object = this;
    this->components.push_back(c);
    this->components[this->components.size() - 1]->start();
}

void GameObject::update(float deltaTime) {
    for (Component *c : this->components) {
        c->update(deltaTime);
    }
    for (GameObject *o : this->children) {
        o->update(deltaTime);
    }
}

void GameObject::cleanup() {
    for (Component *c : this->components) {
        c->cleanup();
    }
}

glm::vec3 GameObject::absPos() {
    if (parent == nullptr) {
        return this->pos;
    } else {
        return this->parent->pos + this->pos;
    }
}

glm::vec3 GameObject::absRot() {
    if (parent == nullptr) {
        return this->rot;
    } else {
        return this->parent->rot + this->rot;
    }
}

glm::vec3 GameObject::absScale() {
    if (parent == nullptr) {
        return this->scale;
    } else {
        return this->parent->scale * this->scale;
    }
}

glm::vec3 GameObject::absForward() {
    glm::vec3 absRot = this->absRot();
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

GameObject *GameObject::setScene(GameObject *scene) {
    if (scene == nullptr)
        scene = this;
    this->scene = scene;
    for (GameObject *c : this->children) {
        c->setScene(scene);
    }
}