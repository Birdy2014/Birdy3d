#pragma once

#include "core/Base.hpp"
#include "render/Forward.hpp"

namespace Birdy3d::physics {

    enum class GenerationMode {
        NONE,
        COPY,
        HULL,
        DECOMPOSITION
    };

    class ConvexMeshGenerators {
    public:
        static std::shared_ptr<render::Model> generate_model(GenerationMode, const render::Model&);

    private:
        static std::shared_ptr<render::Model> copy(const render::Model&);
        static std::shared_ptr<render::Model> hull(const render::Model&);
        static std::shared_ptr<render::Model> decomposition(const render::Model&);
    };

}
