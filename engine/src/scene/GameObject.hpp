#pragma once

#include "scene/Component.hpp"
#include "scene/Transform.hpp"
#include <memory>
#include <vector>

namespace Birdy3d {

    class Scene;
    class Shader;

    class GameObject {
    public:
        std::string name;
        Transform3d transform;
        GameObject* parent = nullptr;
        Scene* scene = nullptr;
        bool hidden = false;

        GameObject(std::string name = "New GameObject", glm::vec3 pos = glm::vec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));

        const std::vector<std::unique_ptr<GameObject>>& children() const { return m_children; }
        void add_child(std::unique_ptr<GameObject>);
        template <class T = GameObject, typename... Args>
        T* add_child(Args... args) {
            static_assert(std::is_base_of<GameObject, T>::value);
            std::unique_ptr<GameObject> object = std::make_unique<T>(args...);
            GameObject* object_ptr = object.get();
            add_child(std::move(object));
            return static_cast<T*>(object_ptr);
        }

        const std::vector<std::unique_ptr<Component>>& components() const { return m_components; }
        void add_component(std::unique_ptr<Component>);
        template <class T, typename... Args>
        T* add_component(Args... args) {
            static_assert(std::is_base_of<Component, T>::value);
            std::unique_ptr<Component> component = std::make_unique<T>(args...);
            Component* component_ptr = component.get();
            add_component(std::move(component));
            return static_cast<T*>(component_ptr);
        }

        virtual void start();
        virtual void update();
        void cleanup();
        glm::vec3 absForward();
        glm::vec3 absRight();
        glm::vec3 absUp();
        void setScene(Scene* scene);

        template <class T>
        std::vector<T*> getComponents(bool hidden = true, bool recursive = false) const {
            std::vector<T*> components;
            if (this->hidden && !hidden)
                return components;
            for (const auto& c : m_components) {
                if (!c->isLoaded())
                    continue;
                T* casted = dynamic_cast<T*>(c.get());
                if (casted) {
                    components.push_back(casted);
                }
            }
            if (recursive) {
                for (const auto& o : m_children) {
                    std::vector<T*> childComponents = o->getComponents<T>(hidden, recursive);
                    components.insert(components.end(), childComponents.begin(), childComponents.end());
                }
            }
            return components;
        }

        template <class T>
        T* getComponent(bool hidden = true, bool recursive = false) const {
            if (this->hidden && !hidden)
                return nullptr;
            for (const auto& c : m_components) {
                if (!c->isLoaded())
                    continue;
                T* casted = dynamic_cast<T*>(c.get());
                if (casted) {
                    return casted;
                }
            }
            if (recursive) {
                for (const auto& o : m_children) {
                    T* c = o->getComponent<T>(hidden, recursive);
                    if (c)
                        return c;
                }
            }
            return nullptr;
        }

    private:
        std::vector<std::unique_ptr<GameObject>> m_children;
        std::vector<std::unique_ptr<Component>> m_components;
    };

}
