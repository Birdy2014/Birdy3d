#pragma once

namespace Birdy3d {

    class Model;

    enum class GenerationMode {
        COPY,
        HULL,
        DECOMPOSITION
    };

    class ConvexMeshGenerators {
    public:
        static Model* generate_model(GenerationMode, Model*);

    private:
        static Model* copy(Model*);
        static Model* hull(Model*);
        static Model* decomposition(Model*);
    };

}
