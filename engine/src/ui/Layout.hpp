#pragma once

#include <glm/glm.hpp>
#include <list>
#include <memory>

namespace Birdy3d {

    class Widget;

    class Layout {
    public:
        virtual void arrange(const std::list<std::unique_ptr<Widget>>& children, glm::vec2 pos, glm::vec2 size) const = 0;
        virtual glm::vec2 minimal_size(const std::list<std::unique_ptr<Widget>>& children) const = 0;
    };

}
