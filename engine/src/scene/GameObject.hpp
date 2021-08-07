#pragma once

#include "core/Base.hpp"
#include "scene/Component.hpp"
#include "scene/Transform.hpp"

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

        const std::vector<std::shared_ptr<GameObject>>& children() const { return m_children; }
        void add_child(std::shared_ptr<GameObject>);
        template <class T = GameObject, typename... Args>
        std::shared_ptr<T> add_child(Args... args) {
            static_assert(std::is_base_of<GameObject, T>::value);
            auto object = std::make_shared<T>(args...);
            add_child(object);
            return std::static_pointer_cast<T>(object);
        }

        const std::vector<std::shared_ptr<Component>>& components() const { return m_components; }
        void add_component(std::shared_ptr<Component>);
        template <class T, typename... Args>
        std::shared_ptr<T> add_component(Args... args) {
            static_assert(std::is_base_of<Component, T>::value);
            auto component = std::make_shared<T>(args...);
            add_component(component);
            return std::static_pointer_cast<T>(component);
        }

        virtual void start();
        virtual void update();
        void cleanup();
        glm::vec3 absForward();
        glm::vec3 absRight();
        glm::vec3 absUp();
        void setScene(Scene* scene);

        template <class T>
        std::vector<std::shared_ptr<T>> get_components(bool hidden = true, bool recursive = false) const {
            std::vector<std::shared_ptr<T>> components;
            if (this->hidden && !hidden)
                return components;
            for (const auto& c : m_components) {
                if (!c->isLoaded())
                    continue;
                auto casted = std::dynamic_pointer_cast<T>(c);
                if (casted) {
                    components.push_back(casted);
                }
            }
            if (recursive) {
                for (const auto& o : m_children) {
                    auto childComponents = o->get_components<T>(hidden, recursive);
                    components.insert(components.end(), childComponents.begin(), childComponents.end());
                }
            }
            return components;
        }

        template <class T>
        std::shared_ptr<T> get_component(bool hidden = true, bool recursive = false) const {
            if (this->hidden && !hidden)
                return nullptr;
            for (const auto& c : m_components) {
                if (!c->isLoaded())
                    continue;
                auto casted = std::dynamic_pointer_cast<T>(c);
                if (casted) {
                    return casted;
                }
            }
            if (recursive) {
                for (const auto& o : m_children) {
                    auto c = o->get_component<T>(hidden, recursive);
                    if (c)
                        return c;
                }
            }
            return nullptr;
        }

        void remove();

    private:
        friend void Component::remove();

        std::vector<std::shared_ptr<GameObject>> m_children;
        std::vector<std::shared_ptr<Component>> m_components;

        void remove_child(GameObject*);
        void remove_component(Component*);
    };

}
