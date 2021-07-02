#pragma once

#include "core/Component.hpp"
#include "core/Transform.hpp"
#include <vector>

namespace Birdy3d {

    class Scene;
    class Shader;

    class GameObject {
    public:
        Transform3d transform;
        GameObject* parent = nullptr;
        Scene* scene = nullptr;
        bool hidden = false;

        GameObject(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
        void addChild(GameObject* c);
        void addComponent(Component* c);
        virtual void start();
        virtual void update();
        void cleanup();
        glm::vec3 absForward();
        glm::vec3 absRight();
        glm::vec3 absUp();
        void setScene(Scene* scene);

        template <class T>
        std::vector<T*> getComponents(bool hidden = true, bool recursive = false) {
            std::vector<T*> components;
            if (this->hidden && !hidden)
                return components;
            for (Component* c : this->components) {
                if (!c->isLoaded())
                    continue;
                T* casted = dynamic_cast<T*>(c);
                if (casted) {
                    components.push_back(casted);
                }
            }
            if (recursive) {
                for (GameObject* o : this->children) {
                    std::vector<T*> childComponents = o->getComponents<T>(hidden, recursive);
                    components.insert(components.end(), childComponents.begin(), childComponents.end());
                }
            }
            return components;
        }

        template <class T>
        T* getComponent(bool hidden = true, bool recursive = false) {
            if (this->hidden && !hidden)
                return nullptr;
            for (Component* c : this->components) {
                if (!c->isLoaded())
                    continue;
                T* casted = dynamic_cast<T*>(c);
                if (casted) {
                    return casted;
                }
            }
            if (recursive) {
                for (GameObject* o : this->children) {
                    T* c = o->getComponent<T>(hidden, recursive);
                    if (c)
                        return c;
                }
            }
            return nullptr;
        }

    private:
        std::vector<GameObject*> children;
        std::vector<Component*> components;
    };

}
