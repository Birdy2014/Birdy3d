#pragma once

#include "core/Logger.hpp"
#include <glm/glm.hpp>

namespace Birdy3d {

    enum class Placement {
        TOP_LEFT,
        BOTTOM_LEFT,
        TOP_RIGHT,
        BOTTOM_RIGHT,
        CENTER_LEFT,
        CENTER_RIGHT,
        TOP_CENTER,
        BOTTOM_CENTER,
        CENTER
    };

    enum class Unit {
        PIXELS,
        PERCENT // Not the best name: 1 is 100%
    };

    class Utils {
    public:
        static glm::vec2 getRelativePosition(glm::vec2 pos, glm::vec2 size, glm::vec2 parentSize, Placement placement, Unit unit) {
            pos = convertToPixels(pos, parentSize, unit);
            glm::vec2 out;

            if (placement == Placement::TOP_LEFT || placement == Placement::BOTTOM_LEFT || placement == Placement::CENTER_LEFT) {
                out.x = pos.x;
            } else if (placement == Placement::TOP_RIGHT || placement == Placement::BOTTOM_RIGHT || placement == Placement::CENTER_RIGHT) {
                out.x = parentSize.x + pos.x;
            } else if (placement == Placement::TOP_CENTER || placement == Placement::BOTTOM_CENTER || placement == Placement::CENTER) {
                out.x = parentSize.x / 2 - size.x / 2 + pos.x;
            }
            if (placement == Placement::TOP_LEFT || placement == Placement::TOP_RIGHT || placement == Placement::TOP_CENTER) {
                out.y = parentSize.y + pos.y;
            } else if (placement == Placement::BOTTOM_LEFT || placement == Placement::BOTTOM_RIGHT || placement == Placement::BOTTOM_CENTER) {
                out.y = pos.y;
            } else if (placement == Placement::CENTER_LEFT || placement == Placement::CENTER_RIGHT || placement == Placement::CENTER) {
                out.y = parentSize.y / 2 - size.y / 2 + pos.y;
            }
            return out;
        }

        static glm::vec2 convertToPixels(glm::vec2 input, glm::vec2 parentSize, Unit unit = Unit::PERCENT) {
            switch (unit) {
            case Unit::PIXELS:
                return input;
            case Unit::PERCENT:
                return input * parentSize;
            }
            Logger::error("Invalid unit");
            return input;
        }
    };

}
