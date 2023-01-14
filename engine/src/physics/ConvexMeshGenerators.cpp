#include "physics/ConvexMeshGenerators.hpp"

#include "core/Logger.hpp"
#include "physics/Collider.hpp"
#include "physics/CollisionMesh.hpp"
#include "physics/IntermediateMesh.hpp"
#include "render/Mesh.hpp"
#include "render/Model.hpp"
#include "utils/Ranges.hpp"

namespace Birdy3d::physics {

    std::shared_ptr<Collider> ConvexMeshGenerators::generate_collider(GenerationMode mode, std::shared_ptr<render::Model> model)
    {
        if (mode == GenerationMode::COPY || mode == GenerationMode::HULL_MESHES || mode == GenerationMode::DECOMPOSITION_MESHES) {
            std::vector<std::unique_ptr<CollisionShape>> meshes;
            for (auto const& m : model->get_meshes()) {
                auto in_mesh = Mesh{m.vertices, m.indices};
                std::optional<Mesh> out_mesh;
                switch (mode) {
                case GenerationMode::COPY:
                    out_mesh = copy(in_mesh);
                    break;
                case GenerationMode::HULL_MESHES:
                    out_mesh = hull(in_mesh);
                    break;
                case GenerationMode::DECOMPOSITION_MESHES:
                    out_mesh = decomposition(in_mesh);
                    break;
                default:
                    return nullptr;
                }
                if (out_mesh.has_value()) {
                    auto points = utils::to_vector(out_mesh->vertices | std::views::transform([](render::Vertex vertex) { return vertex.position; }));
                    meshes.push_back(std::make_unique<CollisionMesh>(points));
                }
            }
            if (meshes.size() == 0)
                return nullptr;
            return std::make_shared<Collider>(model, std::move(meshes));
        } else {
            std::vector<render::Vertex> vertices;
            std::vector<unsigned int> indices;
            unsigned int previous_sizes = 0;
            for (auto const& m : model->get_meshes()) {
                for (auto const& vertex : m.vertices)
                    vertices.push_back(vertex);
                for (auto const& index : m.indices)
                    indices.push_back(previous_sizes + index);
                previous_sizes += m.indices.size();
            }
            Mesh in_mesh{.vertices = vertices, .indices = indices};
            std::vector<std::unique_ptr<CollisionShape>> meshes;
            std::optional<Mesh> out_mesh;
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
            if (out_mesh.has_value()) {
                auto points = utils::to_vector(out_mesh->vertices | std::views::transform([](render::Vertex vertex) { return vertex.position; }));
                meshes.push_back(std::make_unique<CollisionMesh>(points));
            }
            return std::make_shared<Collider>(model, std::move(meshes));
        }
    }

    std::optional<Mesh> ConvexMeshGenerators::copy(Mesh const& mesh)
    {
        return mesh;
    }

    std::optional<Mesh> ConvexMeshGenerators::hull(Mesh const& mesh)
    {
        auto find_furthest_point = [&mesh](glm::vec3 direction) {
            float max = -std::numeric_limits<float>::infinity();
            glm::vec3 furthest_vertex;
            for (auto const& vertex : mesh.vertices) {
                float dot = glm::dot(vertex.position, direction);
                if (dot > max) {
                    max = dot;
                    furthest_vertex = vertex.position;
                }
            }
            return furthest_vertex;
        };

        std::vector<glm::vec3> visited;

        // Create first tetrahedron
        // Get line along one dimension
        glm::vec3 point_min, point_max;
        for (auto direction : {glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)}) {
            point_max = find_furthest_point(direction);
            point_min = find_furthest_point(-direction);
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
        for (auto const& point : mesh.vertices) {
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
        for (auto const& point : mesh.vertices) {
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
        auto intermediate = IntermediateMesh{point_min, point_max, furthest_point_line, furthest_point_plane};

        // TODO: correctly implement quickhull
        /*
        // Get other triangles
        auto add_triangles = [&intermediate](std::vector<glm::vec3>::iterator points, std::size_t point_amount, glm::vec3 direction) {
            // get furthest point in direction and add to intermediate mesh

            // create plane ??

            // sort points by side of plane
            // TODO: on plane: a*x+b*y+c*z+d=0. check sign

            // call add_triangles recursively with sides
        };
        */

        bool done = false;
        while (!done) {
            done = true;
            glm::vec3 furthest;
            // Find Triangle for furthest point
            for (auto it = intermediate.triangles().cbegin(); it != intermediate.triangles().cend(); it++) {
                glm::vec3 normal = it->normal();
                furthest = find_furthest_point(normal);
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

    std::optional<Mesh> ConvexMeshGenerators::decomposition(Mesh const&)
    {
        // TODO: Convex decomposition
        BIRDY3D_TODO
        return {};
    }

}
