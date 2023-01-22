#include "ecs/Transform.hpp"

#include "core/Application.hpp"
#include "ecs/Entity.hpp"
#include "events/EventBus.hpp"
#include "events/TransformChangedEvent.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d::ecs {

    Transform3d::Transform3d(Entity* entity)
        : m_entity(entity)
    { }

    Transform3d::Transform3d(Transform3d const& other)
        : position(other.position)
        , orientation(other.orientation)
        , scale(other.scale)
        , m_entity(other.m_entity)
    { }

    void Transform3d::update(bool changed)
    {
        if (position != m_old_position || orientation != m_old_orientation || scale != m_old_scale) {
            changed = true;
            // Set old values
            m_old_position = position;
            m_old_orientation = orientation;
            m_old_scale = scale;
            // Recalculate local matrix
            m_local_matrix = glm::mat4(1);
            m_local_matrix = glm::translate(m_local_matrix, this->position);
            m_local_matrix = glm::rotate(m_local_matrix, this->orientation.x, glm::vec3(1, 0, 0));
            m_local_matrix = glm::rotate(m_local_matrix, this->orientation.y, glm::vec3(0, 1, 0));
            m_local_matrix = glm::rotate(m_local_matrix, this->orientation.z, glm::vec3(0, 0, 1));
            m_local_matrix = glm::scale(m_local_matrix, this->scale);
            // Send event
            core::Application::event_bus->emit<events::TransformChangedEvent>(m_entity);
        }
        if (changed) {
            // Update matrix
            if (m_entity->parent)
                m_global_matrix = m_entity->parent->transform.global_matrix() * m_local_matrix;
            else
                m_global_matrix = m_local_matrix;
        }
        for (auto child_entity : m_entity->children())
            child_entity->transform.update(changed);
    }

    glm::mat4 Transform3d::global_matrix() const
    {
        return m_global_matrix;
    }

    glm::mat4 Transform3d::inverse_global_matrix() const
    {
        if (!m_inverse_global_matrix.has_value())
            m_inverse_global_matrix = glm::inverse(global_matrix());
        return m_inverse_global_matrix.value();
    }

    glm::mat4 Transform3d::local_matrix() const
    {
        return m_local_matrix;
    }

    glm::vec3 Transform3d::world_position() const
    {
        return local_to_global(glm::vec3(0.0f));
    }

    glm::vec3 Transform3d::world_orientation() const
    {
        if (m_entity->parent)
            return m_entity->parent->transform.orientation + orientation;
        else
            return orientation;
    }

    glm::vec3 Transform3d::world_scale() const
    {
        if (m_entity->parent)
            return m_entity->parent->transform.scale * scale;
        else
            return scale;
    }

    glm::vec3 Transform3d::local_to_global(glm::vec3 local_point) const
    {
        return glm::vec3(global_matrix() * glm::vec4(local_point, 1.0f));
    }

    glm::vec3 Transform3d::global_to_local(glm::vec3 global_point) const
    {
        return glm::vec3(inverse_global_matrix() * glm::vec4(global_point, 1.0f));
    }

    void Transform3d::serialize(serializer::Adapter& adapter)
    {
        adapter("position", position);
        adapter("orientation", orientation);
        adapter("scale", scale);
    }

}
