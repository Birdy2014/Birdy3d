#include "Component.hpp"

Component::Component(GameObject *object) {
    this->object = object;
}

void Component::start() {

}

void Component::update(float deltaTime) {
    
}

void Component::cleanup() {

}

const std::type_info &Component::getTypeid() {

}