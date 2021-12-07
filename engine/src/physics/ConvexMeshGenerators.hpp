#pragma once

#include "core/Base.hpp"

namespace Birdy3d {

    class Mesh;
    class Model;

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
        static std::shared_ptr<Model> generate_model(GenerationMode, const Model&);

    private:
        static std::unique_ptr<Mesh> copy(const Mesh&);
        static std::unique_ptr<Mesh> hull(const Mesh&);
        static std::unique_ptr<Mesh> decomposition(const Mesh&);
    };

}
