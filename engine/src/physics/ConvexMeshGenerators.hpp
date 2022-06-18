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
        static std::shared_ptr<render::Model> generate_model(GenerationMode, const render::Model&);

    private:
        static std::optional<render::Mesh> copy(const render::Mesh&);
        static std::optional<render::Mesh> hull(const render::Mesh&);
        static std::optional<render::Mesh> decomposition(const render::Mesh&);
    };

}
