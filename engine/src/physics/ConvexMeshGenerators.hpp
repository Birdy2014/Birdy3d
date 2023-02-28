#pragma once

#include "core/Base.hpp"
#include "physics/Forward.hpp"
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
        static std::unique_ptr<Collider> generate_collider(GenerationMode, render::Model const&);

    private:
        static std::optional<Mesh> copy(Mesh const&);
        static std::optional<Mesh> hull(Mesh const&);
        static std::optional<Mesh> decomposition(Mesh const&);
    };

}
