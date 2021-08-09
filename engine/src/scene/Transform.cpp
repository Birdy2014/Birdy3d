#include "scene/Transform.hpp"

#include "core/Application.hpp"
#include "events/EventBus.hpp"
#include "events/TransformChangedEvent.hpp"
#include "scene/GameObject.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d {

    Transform3d::Transform3d(GameObject* object)
        : m_object(object) { }

    void Transform3d::post_update() {
        compute_matrix();
        if (m_value_changed) {
            Application::event_bus->emit<TransformChangedEvent>(m_object);
            m_value_changed = false;
        }
    }

    glm::mat4 Transform3d::matrix() {
        if (changed())
            return compute_matrix();
        else
            return m_matrix;
    }

    bool Transform3d::changed(bool updateStatus) {
        if (position != m_old_position || orientation != m_old_orientation || scale != m_old_scale || (m_object->parent && m_object->parent->transform.matrix() != m_old_parent_matrix)) {
            if (updateStatus) {
                m_value_changed = true;
                m_old_position = position;
                m_old_orientation = orientation;
                m_old_scale = scale;
                if (m_object->parent)
                    m_old_parent_matrix = m_object->parent->transform.matrix();
            }
            return true;
        }
        return false;
    }

    glm::mat4 Transform3d::compute_matrix() {
        if (!changed(true))
            return m_matrix;
        glm::mat4 m(1);
        if (m_object->parent)
            m = m * m_object->parent->transform.matrix();
        m = glm::translate(m, this->position);
        m = glm::rotate(m, this->orientation.x, glm::vec3(1, 0, 0));
        m = glm::rotate(m, this->orientation.y, glm::vec3(0, 1, 0));
        m = glm::rotate(m, this->orientation.z, glm::vec3(0, 0, 1));
        m = glm::scale(m, this->scale);
        m_matrix = m;
        return m;
    }

    glm::vec3 Transform3d::worldPosition() {
        return matrix() * glm::vec4(0, 0, 0, 1);
    }

    glm::vec3 Transform3d::worldOrientation() {
        if (m_object->parent)
            return m_object->parent->transform.orientation + orientation;
        else
            return orientation;
    }

    glm::vec3 Transform3d::worldScale() {
        if (m_object->parent)
            return m_object->parent->transform.scale * scale;
        else
            return scale;
    }

}
