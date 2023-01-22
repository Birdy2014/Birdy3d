#pragma once

#include "core/Base.hpp"
#include "ecs/Forward.hpp"
#include "utils/serializer/Adapter.hpp"

namespace Birdy3d::ecs {

    class Transform3d {
    public:
        glm::vec3 position;
        glm::vec3 orientation;
        glm::vec3 scale;

        Transform3d() = delete;
        Transform3d(Entity*);
        Transform3d(Transform3d const&);
        void update(bool changed = false);
        [[nodiscard]] glm::mat4 global_matrix() const;
        [[nodiscard]] glm::mat4 inverse_global_matrix() const;
        [[nodiscard]] glm::mat4 local_matrix() const;
        [[nodiscard]] glm::vec3 world_position() const;
        [[nodiscard]] glm::vec3 world_orientation() const;
        [[nodiscard]] glm::vec3 world_scale() const;
        [[nodiscard]] glm::vec3 local_to_global(glm::vec3) const;
        [[nodiscard]] glm::vec3 global_to_local(glm::vec3) const;
        void serialize(serializer::Adapter&);

    private:
        glm::mat4 m_global_matrix;
        glm::mat4 m_local_matrix;
        mutable std::optional<glm::mat4> m_inverse_global_matrix;
        Entity* m_entity = nullptr;

        // Copies for change detection
        glm::vec3 m_old_position = glm::vec3(0);
        glm::vec3 m_old_orientation = glm::vec3(0);
        glm::vec3 m_old_scale = glm::vec3(0);
    };

}
