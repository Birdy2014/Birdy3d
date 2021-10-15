#include "scene/Transform.hpp"

#include "core/Application.hpp"
#include "events/EventBus.hpp"
#include "events/TransformChangedEvent.hpp"
#include "scene/Entity.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d {

    Transform3d::Transform3d(Entity* entity)
        : m_entity(entity) { }

    Transform3d::Transform3d(const Transform3d& other)
        : position(other.position)
        , orientation(other.orientation)
        , scale(other.scale)
        , m_entity(other.m_entity) { }

    void Transform3d::update(bool changed) {
        if (position != m_old_position || orientation != m_old_orientation || scale != m_old_scale) {
            changed = true;
            // Set old values
            m_old_position = position;
            m_old_orientation = orientation;
            m_old_scale = scale;
            // Send event
            Application::event_bus->emit<TransformChangedEvent>(m_entity);
        }
        if (changed) {
            // Update matrix
            m_matrix = glm::mat4(1);
            if (m_entity->parent)
                m_matrix = m_matrix * m_entity->parent->transform.matrix();
            m_matrix = glm::translate(m_matrix, this->position);
            m_matrix = glm::rotate(m_matrix, this->orientation.x, glm::vec3(1, 0, 0));
            m_matrix = glm::rotate(m_matrix, this->orientation.y, glm::vec3(0, 1, 0));
            m_matrix = glm::rotate(m_matrix, this->orientation.z, glm::vec3(0, 0, 1));
            m_matrix = glm::scale(m_matrix, this->scale);
        }
        for (auto child_entity : m_entity->children())
            child_entity->transform.update(changed);
    }

    glm::mat4 Transform3d::matrix() {
        return m_matrix;
    }

    glm::vec3 Transform3d::world_position() {
        return matrix() * glm::vec4(0, 0, 0, 1);
    }

    glm::vec3 Transform3d::world_orientation() {
        if (m_entity->parent)
            return m_entity->parent->transform.orientation + orientation;
        else
            return orientation;
    }

    glm::vec3 Transform3d::world_scale() {
        if (m_entity->parent)
            return m_entity->parent->transform.scale * scale;
        else
            return scale;
    }

}
