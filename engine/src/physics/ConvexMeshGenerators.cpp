#include "physics/ConvexMeshGenerators.hpp"

#include "core/Logger.hpp"
#include "render/Mesh.hpp"
#include "render/Model.hpp"

namespace Birdy3d::physics {

    std::shared_ptr<render::Model> ConvexMeshGenerators::generate_model(GenerationMode mode, const render::Model& model) {
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

    std::shared_ptr<render::Model> ConvexMeshGenerators::copy(const render::Model& model) {
        std::vector<std::unique_ptr<render::Mesh>> meshes;
        for (const auto& m : model.get_meshes()) {
            std::vector<render::Vertex> vertices = m->vertices;
            std::vector<unsigned int> indices = m->indices;
            meshes.push_back(std::make_unique<render::Mesh>(vertices, indices));
        }
        auto new_model = std::make_shared<render::Model>(meshes);
        return new_model;
    }

    std::shared_ptr<render::Model> ConvexMeshGenerators::hull(const render::Model&) {
        // TODO: Quickhull
        BIRDY3D_TODO
        return nullptr;
    }

    std::shared_ptr<render::Model> ConvexMeshGenerators::decomposition(const render::Model&) {
        // TODO: Convex decomposition
        BIRDY3D_TODO
        return nullptr;
    }

}
