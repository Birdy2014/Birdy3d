#pragma once

#include "core/Base.hpp"

namespace Birdy3d {

    class Model;

    class PrimitiveGenerator {
    public:
        static std::shared_ptr<Model> generate_plane();
        static std::shared_ptr<Model> generate_cube();
        static std::shared_ptr<Model> generate_uv_sphere(unsigned int resolution);
        static std::shared_ptr<Model> generate_ico_sphere(unsigned int resolution);
    };

}
