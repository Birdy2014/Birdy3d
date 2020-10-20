#include "GameObject.hpp"

GameObject::GameObject(GameObject *parent, Shader *s, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) {
    this->parent = parent;
    this->scene = this->getScene();
    this->shader = s;
    this->pos = pos;
    this->rot = rot;
    this->scale = scale;
}

void GameObject::addChild(GameObject c) {
    this->children.push_back(c);
}

void GameObject::addComponent(Component *c) {
    this->components.push_back(c);
    this->components[this->components.size() - 1]->start();
}

void GameObject::update(float deltaTime) {
    for (Component *c : this->components) {
        c->update(deltaTime);
    }
    for (GameObject o : this->children) {
        o.update(deltaTime);
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

GameObject *GameObject::getScene() {
    if (this->parent == nullptr) {
        return this;
    } else {
        return this->parent->getScene();
    }
}