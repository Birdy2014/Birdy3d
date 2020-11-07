#ifndef BIRDY3D_COMPONENT_HPP
#define BIRDY3D_COMPONENT_HPP

class GameObject;

class Component {
public:
    GameObject* object = nullptr;

    Component() {};
    virtual void start() {};
    virtual void update(float deltaTime) {};
    virtual void cleanup() {};
};

#endif