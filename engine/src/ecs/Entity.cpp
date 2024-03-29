#include "ecs/Entity.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d::ecs {

    Entity::Entity(std::string name, glm::vec3 position, glm::vec3 orientation, glm::vec3 scale)
        : name(name)
    {
        transform.position = position;
        transform.orientation = orientation;
        transform.scale = scale;
    }

    std::shared_ptr<Entity> Entity::clone()
    {
        serializer::Object object;
        serializer::Adapter save_adapter(&object, serializer::Adapter::Mode::SAVE);
        save_adapter(name, *this);
        serializer::PointerRegistry::clear();

        auto target = std::make_shared<Entity>();
        serializer::Adapter load_adapter(&object, serializer::Adapter::Mode::LOAD);
        load_adapter(name, *target.get());
        serializer::PointerRegistry::clear();
        return target;
    }

    void Entity::add_child(std::shared_ptr<Entity> c)
    {
        c->parent = this;
        c->set_scene(scene);
        m_children.push_back(std::move(c));
    }

    void Entity::add_child_at(std::size_t index, std::shared_ptr<Entity> child)
    {
        assert(index <= m_children.size());
        child->parent = this;
        child->set_scene(scene);
        child->transform.update(true);
        m_children.insert(std::begin(m_children) + index, child);
    }

    void Entity::add_component(std::shared_ptr<Component> c)
    {
        c->entity = this;
        m_components.push_back(std::move(c));
    }

    void Entity::remove_child(Entity* to_remove)
    {
        if (std::find_if(m_children.begin(), m_children.end(), [&](std::shared_ptr<Entity> entity) { return entity.get() == to_remove; }) != m_children.end())
            to_remove->cleanup();
        m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [&](std::shared_ptr<Entity> child) { return child.get() == to_remove; }), m_children.end());
    }

    void Entity::remove_component(Component* to_remove)
    {
        if (std::find_if(m_components.begin(), m_components.end(), [&](std::shared_ptr<Component> component) { return component.get() == to_remove; }) != m_components.end())
            to_remove->external_cleanup();
        m_components.erase(std::remove_if(m_components.begin(), m_components.end(), [&](std::shared_ptr<Component> component) { return component.get() == to_remove; }), m_components.end());
    }

    void Entity::remove()
    {
        if (parent)
            parent->remove_child(this);
    }

    std::shared_ptr<Entity> Entity::move_child_out(Entity* to_move)
    {
        auto child_iterator = std::find_if(m_children.begin(), m_children.end(), [&](std::shared_ptr<Entity> entity) { return entity.get() == to_move; });
        if (child_iterator == m_children.end())
            return {};

        auto child = *child_iterator;

        m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [&](std::shared_ptr<Entity> child) { return child.get() == to_move; }), m_children.end());
        return child;
    }

    void Entity::start()
    {
        std::sort(m_components.begin(), m_components.end(), [](std::shared_ptr<Component> const& a, std::shared_ptr<Component> const& b) {
            return a->priority() < b->priority();
        });
        for (auto const& c : m_components) {
            c->entity = this;
            c->external_start();
        }
        for (auto const& o : m_children) {
            o->parent = this;
            o->start();
        }
    }

    void Entity::update()
    {
        if (hidden)
            return;

        for (auto const& c : m_components) {
            c->external_update();
        }
        for (auto const& o : m_children) {
            o->update();
        }
    }

    void Entity::post_update()
    {
        if (hidden)
            return;

        for (auto const& o : m_children) {
            o->post_update();
        }
    }

    void Entity::cleanup()
    {
        for (auto const& c : m_components) {
            c->external_cleanup();
        }
    }

    glm::vec3 Entity::world_forward()
    {
        glm::vec3 world_rot = this->transform.world_orientation();
        glm::vec3 forward;
        forward.x = cos(world_rot.y) * cos(world_rot.x);
        forward.y = sin(world_rot.x);
        forward.z = sin(world_rot.y) * cos(world_rot.x);
        return glm::normalize(forward);
    }

    glm::vec3 Entity::world_right()
    {
        return glm::normalize(glm::cross(this->world_forward(), glm::vec3(0, 1, 0)));
    }

    glm::vec3 Entity::world_up()
    {
        return glm::normalize(glm::cross(this->world_right(), this->world_forward()));
    }

    void Entity::set_scene(Scene* scene)
    {
        this->scene = scene;
        for (auto const& c : m_children) {
            c->set_scene(scene);
        }
    }

    bool Entity::is_descendant_of(Entity const& other) const
    {
        for (auto current_entity = parent; current_entity; current_entity = current_entity->parent) {
            if (current_entity == &other)
                return true;
        }
        return false;
    }

    void Entity::serialize(serializer::Adapter& adapter)
    {
        adapter("name", name);
        adapter("hidden", hidden);
        adapter("transform", transform);
        adapter("components", m_components);
        adapter("children", m_children);
    }

    BIRDY3D_REGISTER_TYPE_DEF(Entity);

}
