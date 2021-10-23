#pragma once

#include "core/Base.hpp"
#include "ecs/Component.hpp"
#include "ecs/Transform.hpp"
#include "utils/serializer/Adapter.hpp"

namespace Birdy3d {

    class Scene;
    class Shader;

    class Entity {
    public:
        std::string name;
        Transform3d transform = Transform3d(this);
        Entity* parent = nullptr;
        Scene* scene = nullptr;
        bool hidden = false;

        Entity(std::string name = "New Entity", glm::vec3 pos = glm::vec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
        virtual ~Entity() = default;

        const std::vector<std::shared_ptr<Entity>>& children() const { return m_children; }
        void add_child(std::shared_ptr<Entity>);
        template <class T = Entity, typename... Args>
        std::shared_ptr<T> add_child(Args... args) {
            static_assert(std::is_base_of<Entity, T>::value);
            auto entity = std::make_shared<T>(args...);
            add_child(entity);
            return std::static_pointer_cast<T>(entity);
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
        virtual void post_update();
        void cleanup();
        glm::vec3 world_forward();
        glm::vec3 world_right();
        glm::vec3 world_up();
        void set_scene(Scene* scene);

        template <class T>
        void get_components(std::vector<std::shared_ptr<T>>& components, bool hidden = true, bool recursive = false) const {
            if (this->hidden && !hidden)
                return;
            for (const auto& c : m_components) {
                if (!c->loaded())
                    continue;
                auto casted = std::dynamic_pointer_cast<T>(c);
                if (casted) {
                    components.push_back(casted);
                }
            }
            if (recursive) {
                for (const auto& o : m_children) {
                    o->get_components<T>(components, hidden, recursive);
                }
            }
            return;
        }

        template <class T>
        std::vector<std::shared_ptr<T>> get_components(bool hidden = true, bool recursive = false) const {
            std::vector<std::shared_ptr<T>> components;
            get_components<T>(components, hidden, recursive);
            return components;
        }

        template <class T>
        std::shared_ptr<T> get_component(bool hidden = true, bool recursive = false) const {
            if (this->hidden && !hidden)
                return nullptr;
            for (const auto& c : m_components) {
                if (!c->loaded())
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
        virtual void serialize(serializer::Adapter&);

    private:
        friend void Component::remove();

        std::vector<std::shared_ptr<Entity>> m_children;
        std::vector<std::shared_ptr<Component>> m_components;

        void remove_child(Entity*);
        void remove_component(Component*);

        BIRDY3D_REGISTER_TYPE_DEC(Entity);
    };

}
