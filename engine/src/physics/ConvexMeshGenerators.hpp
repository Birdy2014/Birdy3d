#pragma once

#include "core/Base.hpp"

namespace Birdy3d {

    class Model;

    enum class GenerationMode {
        NONE,
        COPY,
        HULL,
        DECOMPOSITION
    };

    class ConvexMeshGenerators {
    public:
        static std::shared_ptr<Model> generate_model(GenerationMode, const Model&);

    private:
        static std::shared_ptr<Model> copy(const Model&);
        static std::shared_ptr<Model> hull(const Model&);
        static std::shared_ptr<Model> decomposition(const Model&);
    };

}
