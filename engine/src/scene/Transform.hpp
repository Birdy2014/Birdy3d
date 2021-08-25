#pragma once

#include "core/Base.hpp"

namespace Birdy3d {

    class GameObject;

    class Transform3d {
    public:
        glm::vec3 position;
        glm::vec3 orientation;
        glm::vec3 scale;

        Transform3d() = delete;
        Transform3d(GameObject*);
        void post_update();
        glm::mat4 matrix();
        glm::vec3 worldPosition();
        glm::vec3 worldOrientation();
        glm::vec3 worldScale();
        template <class Archive>
        void serialize(Archive& ar) {
            ar(cereal::make_nvp("position", position));
            ar(cereal::make_nvp("orientation", orientation));
            ar(cereal::make_nvp("scale", scale));
        }

    private:
        glm::mat4 m_matrix;
        GameObject* m_object = nullptr;
        bool m_value_changed = false;

        // Copies for change detection
        glm::vec3 m_old_position = glm::vec3(0);
        glm::vec3 m_old_orientation = glm::vec3(0);
        glm::vec3 m_old_scale = glm::vec3(0);
        glm::mat4 m_old_parent_matrix = glm::mat4(1);

        bool changed(bool updateStatus = false);
        glm::mat4 compute_matrix();
    };

}
