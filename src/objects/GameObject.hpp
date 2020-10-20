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
    GameObject *scene;

    GameObject(GameObject *parent, Shader *s, glm::vec3 pos = glm::vec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
    void addChild(GameObject c);
    void addComponent(Component *c);
    void update(float deltaTime);
    void cleanup();
    glm::vec3 absPos();
    glm::vec3 absRot();
    glm::vec3 absScale();
    GameObject *getScene();

    template<class T>
    std::vector<T*> getComponents(bool recursive = false) {
        std::vector<T*> components;
        for (GameObject o : this->children) {
            std::vector<T*> childComponents = o.getComponents<T>(true);
            components.insert(components.end(), childComponents.begin(), childComponents.end());
        }
        for (Component *c : this->components) {
            if (c->getTypeid() == typeid(T)) {
                components.push_back((T*)c);
            }
        }
        return components;
    }

    template<class T>
    T *getComponent() {
        for (Component *c : this->components) {
        std::cout << typeid(c).name() << " " << typeid(T).name() << std::endl;
            if (c->getTypeid() == typeid(T)) {
                return (T*)c;
            }
        }
        std::cout << "Size: " << this->components.size() << " Not found" << std::endl;
        return nullptr;
    }

private:
    GameObject *parent;
    std::vector<GameObject> children;
    std::vector<Component*> components;
};

#endif