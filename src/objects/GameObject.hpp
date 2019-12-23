#ifndef BIRDY3D_GAMEOBJECT_HPP
#define BIRDY3D_GAMEOBJECT_HPP

#include <vector>
#include "../render/Shader.hpp"
#include "Component.hpp"

class GameObject {
public:
    Shader *shader;
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;

    GameObject(GameObject *parent, Shader *s, glm::vec3 pos = glm::vec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
    void addChild(GameObject c);
    void addComponent(Component *c);
    void update(float deltaTime);
    void cleanup();
    glm::vec3 absPos();
    glm::vec3 absRot();
    glm::vec3 absScale();

private:
    GameObject *parent;
    std::vector<GameObject> children;
    std::vector<Component*> components;
};

#endif