#ifndef BIRDY3D_COMPONENT_HPP
#define BIRDY3D_COMPONENT_HPP

class GameObject;

class Component {
public:
    Component(GameObject *object);
    virtual void start();
    virtual void update(float deltaTime);
    virtual void renderDepth();
    virtual void cleanup();
    virtual const std::type_info &getTypeid();

protected:
    GameObject* object;
};

#endif