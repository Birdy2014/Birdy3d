#pragma once

#include "core/Base.hpp"
#include "render/Forward.hpp"
#include <optional>

namespace Birdy3d::physics {

    enum class GenerationMode {
        NONE,
        COPY,
        HULL_MODEL,
        HULL_MESHES,
        DECOMPOSITION_MODEL,
        DECOMPOSITION_MESHES
    };

    class ConvexMeshGenerators {
    public:
        static std::shared_ptr<render::Model> generate_model(GenerationMode, render::Model const&);

    private:
        static std::optional<render::Mesh> copy(render::Mesh const&);
        static std::optional<render::Mesh> hull(render::Mesh const&);
        static std::optional<render::Mesh> decomposition(render::Mesh const&);
    };

}
