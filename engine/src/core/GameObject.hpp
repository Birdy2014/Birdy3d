#ifndef BIRDY3D_GAMEOBJECT_HPP
#define BIRDY3D_GAMEOBJECT_HPP

#include <vector>
#include "core/Component.hpp"
#include "render/Shader.hpp"

class GameObject {
public:
    Shader *shader;
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;
    GameObject *parent = nullptr;
    GameObject *scene = nullptr;

    GameObject(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
    void addChild(GameObject *c);
    void addComponent(Component *c);
    void update(float deltaTime);
    void cleanup();
    glm::mat4 absTransform();
    glm::vec3 absPos();
    glm::vec3 absRot();
    glm::vec3 absScale();
    glm::vec3 absForward();
    glm::vec3 absRight();
    glm::vec3 absUp();
    GameObject *setScene(GameObject *scene = nullptr);

    template<class T>
    std::vector<T*> getComponents(bool recursive = false) {
        std::vector<T*> components;
        for (Component *c : this->components) {
            T *casted = dynamic_cast<T*>(c);
            if (casted) {
                components.push_back(casted);
            }
        }
        if (recursive) {
            for (GameObject *o : this->children) {
                std::vector<T*> childComponents = o->getComponents<T>(true);
                components.insert(components.end(), childComponents.begin(), childComponents.end());
            }
        }
        return components;
    }

    template<class T>
    T *getComponent() {
        for (Component *c : this->components) {
            T *casted = dynamic_cast<T*>(c);
            if (casted) {
                return casted;
            }
        }
        return nullptr;
    }

private:
    std::vector<GameObject*> children;
    std::vector<Component*> components;
};

#endif