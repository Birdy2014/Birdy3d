#include "physics/ConvexMeshGenerators.hpp"

#include "core/Logger.hpp"
#include "physics/IntermediateMesh.hpp"
#include "render/Mesh.hpp"
#include "render/Model.hpp"

namespace Birdy3d::physics {

    std::shared_ptr<render::Model> ConvexMeshGenerators::generate_model(GenerationMode mode, const render::Model& model) {
        if (mode == GenerationMode::COPY || mode == GenerationMode::HULL_MESHES || mode == GenerationMode::DECOMPOSITION_MESHES) {
            std::vector<render::Mesh> meshes;
            for (const auto& m : model.get_meshes()) {
                std::optional<render::Mesh> out_mesh;
                switch (mode) {
                case GenerationMode::COPY:
                    out_mesh = copy(m);
                    break;
                case GenerationMode::HULL_MESHES:
                    out_mesh = hull(m);
                    break;
                case GenerationMode::DECOMPOSITION_MESHES:
                    out_mesh = decomposition(m);
                    break;
                default:
                    return nullptr;
                }
                if (out_mesh.has_value())
                    meshes.push_back(std::move(out_mesh.value()));
            }
            if (meshes.size() == 0)
                return nullptr;
            return std::make_shared<render::Model>(meshes);
        } else {
            std::vector<render::Vertex> vertices;
            std::vector<unsigned int> indices;
            unsigned int previous_sizes = 0;
            for (const auto& m : model.get_meshes()) {
                for (const auto& vertex : m.vertices)
                    vertices.push_back(vertex);
                for (const auto& index : m.indices)
                    indices.push_back(previous_sizes + index);
                previous_sizes += m.indices.size();
            }
            render::Mesh in_mesh(vertices, indices);
            std::optional<render::Mesh> out_mesh;
            switch (mode) {
            case GenerationMode::HULL_MODEL:
                out_mesh = hull(in_mesh);
                break;
            case GenerationMode::DECOMPOSITION_MODEL:
                out_mesh = decomposition(in_mesh);
                break;
            default:
                return nullptr;
            }
            if (!out_mesh.has_value())
                return nullptr;
            return std::make_shared<render::Model>(std::move(out_mesh.value()));
        }
    }

    std::optional<render::Mesh> ConvexMeshGenerators::copy(const render::Mesh& mesh) {
        std::vector<render::Vertex> vertices = mesh.vertices;
        std::vector<unsigned int> indices = mesh.indices;
        return render::Mesh { vertices, indices };
    }

    std::optional<render::Mesh> ConvexMeshGenerators::hull(const render::Mesh& mesh) {
        std::vector<glm::vec3> visited;

        // Create first tetrahedron
        // Get line along one dimension
        glm::vec3 point_min, point_max;
        for (auto direction : { glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1) }) {
            point_max = mesh.find_furthest_point(direction);
            point_min = mesh.find_furthest_point(-direction);
            if (point_max != point_min)
                break;
        }
        if (point_max == point_min) {
            core::Logger::warn("ConvexMeshGenerators::hull failed, because the render::Mesh is not 3D.");
            return {};
        }
        // Get furthest point from line
        glm::vec3 furthest_point_line = point_max;
        float furthest_distance = 0;
        for (const auto& point : mesh.vertices) {
            // Calculate distance between furthest_point and line
            float distance = std::abs(glm::length(glm::cross(point_max - point_min, point.position - point_min)) / glm::length(point_max - point_min));
            if (distance > furthest_distance) {
                furthest_point_line = point.position;
                furthest_distance = distance;
            }
        }
        if (furthest_distance == 0) {
            core::Logger::warn("ConvexMeshGenerators::hull failed, because the render::Mesh is 1D.");
            return {};
        }
        // Get furthest point from plane
        glm::vec3 furthest_point_plane = point_max;
        furthest_distance = 0;
        Triangle plane_triangle(point_min, point_max, furthest_point_line);
        glm::vec3 n = plane_triangle.normal();
        float d = glm::dot(n, point_min);
        for (const auto& point : mesh.vertices) {
            float distance = glm::dot(glm::vec4(n, d), glm::vec4(point.position, -1.0f));
            if (std::abs(distance) > std::abs(furthest_distance)) {
                furthest_point_plane = point.position;
                furthest_distance = distance;
            }
        }
        if (furthest_distance == 0) {
            core::Logger::warn("ConvexMeshGenerators::hull failed, because the render::Mesh is 2D.");
            return {};
        }

        visited.push_back(point_min);
        visited.push_back(point_max);
        visited.push_back(furthest_point_line);
        visited.push_back(furthest_point_plane);
        auto intermediate = IntermediateMesh { point_min, point_max, furthest_point_line, furthest_point_plane };

        // Get other triangles
        bool done = false;
        while (!done) {
            done = true;
            glm::vec3 furthest;
            // Find Triangle for furthest point
            for (auto it = intermediate.triangles().cbegin(); it != intermediate.triangles().cend(); it++) {
                glm::vec3 normal = it->normal();
                furthest = mesh.find_furthest_point(normal);
                if (std::find(visited.cbegin(), visited.cend(), furthest) == visited.cend()) {
                    done = false;
                    break;
                }
            }
            visited.push_back(furthest);
            intermediate.expand(furthest);
        }

        return intermediate.to_mesh();
    }

    std::optional<render::Mesh> ConvexMeshGenerators::decomposition(const render::Mesh&) {
        // TODO: Convex decomposition
        BIRDY3D_TODO
        return {};
    }

}
