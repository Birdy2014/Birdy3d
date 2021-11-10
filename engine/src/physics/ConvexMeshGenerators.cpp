#include "physics/ConvexMeshGenerators.hpp"

#include "core/Logger.hpp"
#include "render/Mesh.hpp"
#include "render/Model.hpp"

namespace Birdy3d {

    int ConvexMeshGenerators::limit = 4;

    struct Triangle {
        glm::vec3 a, b, c;

        Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c)
            : a(a)
            , b(b)
            , c(c) { }

        glm::vec3 normal() const {
            glm::vec3 side1 = b - a;
            glm::vec3 side2 = a - c;
            return glm::cross(side1, side2);
        }

        bool operator==(const Triangle& other) const {
            return (a == other.a && b == other.b && c == other.c) || (a == other.b && b == other.c && c == other.a) || (a == other.c && b == other.a && c == other.b);
        }
    };

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

    std::shared_ptr<Model> ConvexMeshGenerators::hull(const Model& model) {
        std::vector<std::unique_ptr<Mesh>> meshes;
        for (const auto& mesh : model.get_meshes()) {
            std::vector<glm::vec3> visited;
            std::vector<Triangle> triangles;
            // Get first tetrahedron
            // TODO: Improve creation of first tetrahedron
            glm::vec3 a = mesh->find_furthest_point(glm::vec3(1, 0, 0));
            glm::vec3 b = mesh->find_furthest_point(glm::vec3(-1, 0, 0));
            glm::vec3 c = mesh->find_furthest_point(glm::vec3(0, 1, 0));
            glm::vec3 d = mesh->find_furthest_point(glm::vec3(0, 0, 1));
            visited.push_back(a);
            visited.push_back(b);
            visited.push_back(c);
            visited.push_back(d);
            triangles.emplace_back(a, c, b);
            triangles.emplace_back(b, d, a);
            triangles.emplace_back(b, c, d);
            triangles.emplace_back(a, d, c);

            // Get other triangles
            bool done = false;
            while (!done) {
                done = true;
                for (std::size_t i = 0; i < triangles.size(); i++) {
                    // For debugging
                    if (visited.size() > limit)
                        break;

                    auto triangle = triangles[i];
                    glm::vec3 normal = triangle.normal();
                    glm::vec3 furthest = mesh->find_furthest_point(normal);
                    if (std::find(visited.cbegin(), visited.cend(), furthest) != visited.cend()) {
                        // Some Triangles already exist
                        Triangle t1(furthest, triangle.a, triangle.b);
                        if (std::find(triangles.cbegin(), triangles.cend(), t1) == triangles.cend())
                            triangles.push_back(t1);
                        Triangle t2(furthest, triangle.b, triangle.c);
                        if (std::find(triangles.cbegin(), triangles.cend(), t1) == triangles.cend())
                            triangles.push_back(t2);
                        Triangle t3(furthest, triangle.c, triangle.a);
                        if (std::find(triangles.cbegin(), triangles.cend(), t1) == triangles.cend())
                            triangles.push_back(t3);
                        triangles.erase(triangles.begin() + i);
                        continue;
                    }
                    done = false;
                    visited.push_back(furthest);
                    // Create new triangles
                    triangles.emplace_back(furthest, triangle.a, triangle.b);
                    triangles.emplace_back(furthest, triangle.b, triangle.c);
                    //triangles.emplace_back(furthest, triangle.c, triangle.a);
                    triangle.b = triangle.c;
                    triangle.c = triangle.a;
                    triangle.a = furthest;

                    /*
                     * FIXME: It is not enough to create 3 new triangles from a triangle.
                     * From each newly visited point, the horizon should be calculated and thr triangles sould be created from it.
                     * Or, maybe, edges should be expanded, which should create 4 new triangles from the two existing triangles of the edge.
                     */
                }
            }

            // Convert triangles to mesh
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            unsigned int current_index = 0;
            for (const auto& triangle : triangles) {
                indices.push_back(current_index++);
                indices.push_back(current_index++);
                indices.push_back(current_index++);
                glm::vec3 normal = triangle.normal();
                Vertex v1;
                v1.position = triangle.a;
                v1.normal = normal;
                vertices.push_back(v1);
                Vertex v2;
                v2.position = triangle.b;
                v2.normal = normal;
                vertices.push_back(v2);
                Vertex v3;
                v3.position = triangle.c;
                v3.normal = normal;
                vertices.push_back(v3);
            }
            meshes.push_back(std::make_unique<Mesh>(vertices, indices));
        }
        return std::make_shared<Model>(meshes);
    }

    std::shared_ptr<Model> ConvexMeshGenerators::decomposition(const Model&) {
        // TODO: Convex decomposition
        BIRDY3D_TODO
        return nullptr;
    }

}
