#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <vector>

namespace Birdy3d::physics {

    class CollisionShape {
    public:
        virtual ~CollisionShape() = default;
        [[nodiscard]] virtual glm::vec3 find_furthest_point(const glm::vec3 direction) const = 0;
    };

}
