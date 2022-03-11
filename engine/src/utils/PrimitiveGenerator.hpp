#pragma once

#include "core/Base.hpp"
#include "render/Forward.hpp"

namespace Birdy3d::utils {

    class PrimitiveGenerator {
    public:
        static std::shared_ptr<render::Model> generate_plane(unsigned int resolution);
        static std::shared_ptr<render::Model> generate_quad_plane(unsigned int resolution);
        static std::shared_ptr<render::Model> generate_cube();
        static std::shared_ptr<render::Model> generate_uv_sphere(unsigned int resolution);
        static std::shared_ptr<render::Model> generate_ico_sphere(unsigned int resolution);
    };

}
