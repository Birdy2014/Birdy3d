#include "physics/ConvexMeshGenerators.hpp"

#include "core/Logger.hpp"
#include "render/Mesh.hpp"
#include "render/Model.hpp"

namespace Birdy3d {

    std::shared_ptr<Model> ConvexMeshGenerators::generate_model(GenerationMode mode, const Model& model) {
        switch (mode) {
        case GenerationMode::COPY:
            return copy(model);
        case GenerationMode::HULL:
            return hull(model);
        case GenerationMode::DECOMPOSITION:
            return decomposition(model);
        default:
            return nullptr;
        }
    }

    std::shared_ptr<Model> ConvexMeshGenerators::copy(const Model& model) {
        std::vector<std::unique_ptr<Mesh>> meshes;
        for (const auto& m : model.get_meshes()) {
            std::vector<Vertex> vertices = m->vertices;
            std::vector<unsigned int> indices = m->indices;
            meshes.push_back(std::make_unique<Mesh>(vertices, indices));
        }
        auto new_model = std::make_shared<Model>(meshes);
        return new_model;
    }

    std::shared_ptr<Model> ConvexMeshGenerators::hull(const Model&) {
        // TODO: Quickhull
        BIRDY3D_TODO
        return nullptr;
    }

    std::shared_ptr<Model> ConvexMeshGenerators::decomposition(const Model&) {
        // TODO: Convex decomposition
        BIRDY3D_TODO
        return nullptr;
    }

}
