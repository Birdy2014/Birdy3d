#pragma once

#include <glm/glm.hpp>
#include <string>

namespace Birdy3d {

    class Theme {
    public:
        const glm::vec4 color_fg;
        const glm::vec4 color_bg;
        const glm::vec4 color_border;

        Theme(glm::vec4 color_fg, glm::vec4 color_bg, glm::vec4 color_border);
        Theme(const std::string& color_fg, const std::string& color_bg, const std::string& color_border);
        static glm::vec4 parseColor(const std::string& colorString);

        // TODO: load theme from file
    };

}
