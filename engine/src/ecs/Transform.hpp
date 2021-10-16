#pragma once

#include "core/Base.hpp"

namespace Birdy3d {

    class Entity;

    class Transform3d {
    public:
        glm::vec3 position;
        glm::vec3 orientation;
        glm::vec3 scale;

        Transform3d() = delete;
        Transform3d(Entity*);
        Transform3d(const Transform3d&);
        void update(bool changed = false);
        glm::mat4 matrix();
        glm::vec3 world_position();
        glm::vec3 world_orientation();
        glm::vec3 world_scale();
        template <class Archive>
        void serialize(Archive& ar) {
            ar(cereal::make_nvp("position", position));
            ar(cereal::make_nvp("orientation", orientation));
            ar(cereal::make_nvp("scale", scale));
        }

    private:
        glm::mat4 m_matrix;
        Entity* m_entity = nullptr;

        // Copies for change detection
        glm::vec3 m_old_position = glm::vec3(0);
        glm::vec3 m_old_orientation = glm::vec3(0);
        glm::vec3 m_old_scale = glm::vec3(0);
    };

}
