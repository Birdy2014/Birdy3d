#include "physics/ConvexMeshGenerators.hpp"

#include "core/Logger.hpp"

namespace Birdy3d {

    Model* ConvexMeshGenerators::generate_model(GenerationMode mode, Model* model) {
        switch (mode) {
        case GenerationMode::COPY:
            return copy(model);
        case GenerationMode::HULL:
            return hull(model);
        case GenerationMode::DECOMPOSITION:
            return decomposition(model);
        }
        return nullptr;
    }

    Model* ConvexMeshGenerators::copy(Model* model) {
        return model;
    }

    Model* ConvexMeshGenerators::hull(Model*) {
        // TODO
        BIRDY3D_TODO
        return nullptr;
    }

    Model* ConvexMeshGenerators::decomposition(Model*) {
        // TODO
        BIRDY3D_TODO
        return nullptr;
    }

}
