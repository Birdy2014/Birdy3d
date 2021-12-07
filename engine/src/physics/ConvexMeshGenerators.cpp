#include "physics/ConvexMeshGenerators.hpp"

#include "core/Logger.hpp"
#include "render/Mesh.hpp"
#include "render/Model.hpp"

namespace Birdy3d {

    struct Triangle {
        enum class Edge {
            NONE,
            AB,
            BC,
            CA
        };

        glm::vec3 a, b, c;

        // Adjecant triangles
        Triangle *tab = nullptr, *tbc = nullptr, *tca = nullptr;

        Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c)
            : a(a)
            , b(b)
            , c(c) { }

        glm::vec3 normal() const {
            glm::vec3 side1 = b - a;
            glm::vec3 side2 = c - a;
            return glm::cross(side1, side2);
        }

        bool outside(const glm::vec3& point) const {
            glm::vec3 n = normal();
            float d = glm::dot(n, a);
            return glm::dot(glm::vec4(n, d), glm::vec4(point, -1.0f)) > 0;
        }

        Edge get_edge(const glm::vec3& other_a, const glm::vec3& other_b) const {
            // clang-format off
            if ((other_a == a && other_b == b) || (other_a == b && other_b == a))
                return Edge::AB;
            if ((other_a == b && other_b == c) || (other_a == c && other_b == b))
                return Edge::BC;
            if ((other_a == c && other_b == a) || (other_a == a && other_b == c))
                return Edge::CA;
            return Edge::NONE;
            // clang-format on
        }
    };

    std::shared_ptr<Model> ConvexMeshGenerators::generate_model(GenerationMode mode, const Model& model) {
        if (mode == GenerationMode::COPY || mode == GenerationMode::HULL_MESHES || mode == GenerationMode::DECOMPOSITION_MESHES) {
            std::vector<std::unique_ptr<Mesh>> meshes;
            for (const auto& m : model.get_meshes()) {
                std::unique_ptr<Mesh> out_mesh;
                switch (mode) {
                case GenerationMode::COPY:
                    out_mesh = copy(*m);
                    break;
                case GenerationMode::HULL_MESHES:
                    out_mesh = hull(*m);
                    break;
                case GenerationMode::DECOMPOSITION_MESHES:
                    out_mesh = decomposition(*m);
                    break;
                default:
                    return nullptr;
                }
                if (out_mesh)
                    meshes.push_back(std::move(out_mesh));
            }
            if (meshes.size() == 0)
                return nullptr;
            return std::make_shared<Model>(meshes);
        } else {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            unsigned int previous_sizes = 0;
            for (const auto& m : model.get_meshes()) {
                for (const auto& vertex : m->vertices)
                    vertices.push_back(vertex);
                for (const auto& index : m->indices)
                    indices.push_back(previous_sizes + index);
                previous_sizes += m->indices.size();
            }
            Mesh in_mesh(vertices, indices);
            std::unique_ptr<Mesh> out_mesh;
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
            if (!out_mesh)
                return nullptr;
            return std::make_shared<Model>(std::move(out_mesh));
        }
    }

    std::unique_ptr<Mesh> ConvexMeshGenerators::copy(const Mesh& mesh) {
        std::vector<Vertex> vertices = mesh.vertices;
        std::vector<unsigned int> indices = mesh.indices;
        return std::make_unique<Mesh>(vertices, indices);
    }

    std::unique_ptr<Mesh> ConvexMeshGenerators::hull(const Mesh& mesh) {
        std::vector<glm::vec3> visited;
        std::vector<std::unique_ptr<Triangle>> triangles;

        // Helper functions
        auto add_triangle = [&triangles](glm::vec3 a, glm::vec3 b, glm::vec3 c) -> Triangle* {
            auto tri = std::make_unique<Triangle>(a, b, c);
            auto ptr = tri.get();
            triangles.push_back(std::move(tri));
            return ptr;
        };

        auto erase_triangle = [&triangles](std::vector<std::unique_ptr<Triangle>>::iterator it) -> void {
            auto ptr = it->get();
            // clang-format off
            if (ptr->tab) {
                if (ptr->tab->tab == ptr) ptr->tab->tab = nullptr;
                if (ptr->tab->tbc == ptr) ptr->tab->tbc = nullptr;
                if (ptr->tab->tca == ptr) ptr->tab->tca = nullptr;
            }

            if (ptr->tbc) {
                if (ptr->tbc->tab == ptr) ptr->tbc->tab = nullptr;
                if (ptr->tbc->tbc == ptr) ptr->tbc->tbc = nullptr;
                if (ptr->tbc->tca == ptr) ptr->tbc->tca = nullptr;
            }

            if (ptr->tca) {
                if (ptr->tca->tab == ptr) ptr->tca->tab = nullptr;
                if (ptr->tca->tbc == ptr) ptr->tca->tbc = nullptr;
                if (ptr->tca->tca == ptr) ptr->tca->tca = nullptr;
            }
            // clang-format on
            triangles.erase(it);
        };

        auto add_neighbours = [&triangles](std::size_t begin_offset) -> bool {
            for (auto it = triangles.begin() + begin_offset; it != triangles.end(); ++it) {
                if (!(*it)->tab) {
                    for (const auto& triangle : triangles) {
                        if (it->get() == triangle.get())
                            continue;
                        auto edge = triangle->get_edge((*it)->a, (*it)->b);
                        if (edge == Triangle::Edge::NONE)
                            continue;
                        if (edge == Triangle::Edge::AB && !triangle->tab)
                            triangle->tab = it->get();
                        else if (edge == Triangle::Edge::BC && !triangle->tbc)
                            triangle->tbc = it->get();
                        else if (edge == Triangle::Edge::CA && !triangle->tca)
                            triangle->tca = it->get();
                        else
                            return false;
                        (*it)->tab = triangle.get();
                        break;
                    }
                }
                if (!(*it)->tbc) {
                    for (const auto& triangle : triangles) {
                        if (it->get() == triangle.get())
                            continue;
                        auto edge = triangle->get_edge((*it)->b, (*it)->c);
                        if (edge == Triangle::Edge::NONE)
                            continue;
                        if (edge == Triangle::Edge::AB && !triangle->tab)
                            triangle->tab = it->get();
                        else if (edge == Triangle::Edge::BC && !triangle->tbc)
                            triangle->tbc = it->get();
                        else if (edge == Triangle::Edge::CA && !triangle->tca)
                            triangle->tca = it->get();
                        else
                            return false;
                        (*it)->tbc = triangle.get();
                        break;
                    }
                }
                if (!(*it)->tca) {
                    for (const auto& triangle : triangles) {
                        if (it->get() == triangle.get())
                            continue;
                        auto edge = triangle->get_edge((*it)->c, (*it)->a);
                        if (edge == Triangle::Edge::NONE)
                            continue;
                        if (edge == Triangle::Edge::AB && !triangle->tab)
                            triangle->tab = it->get();
                        else if (edge == Triangle::Edge::BC && !triangle->tbc)
                            triangle->tbc = it->get();
                        else if (edge == Triangle::Edge::CA && !triangle->tca)
                            triangle->tca = it->get();
                        else
                            return false;
                        (*it)->tca = triangle.get();
                        break;
                    }
                }
            }
            return true;
        };

        bool done = false;
        // Get first tetrahedron
        {
            // Get line along one dimension
            glm::vec3 point_min, point_max;
            for (auto direction : { glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1) }) {
                point_max = mesh.find_furthest_point(direction);
                point_min = mesh.find_furthest_point(-direction);
                if (point_max != point_min)
                    break;
            }
            if (point_max == point_min) {
                Logger::warn("ConvexMeshGenerators::hull failed, because the Mesh is not 3D.");
                return nullptr;
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
                Logger::warn("ConvexMeshGenerators::hull failed, because the Mesh is 1D.");
                return nullptr;
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
                Logger::warn("ConvexMeshGenerators::hull failed, because the Mesh is 2D.");
                return nullptr;
            }
            // Create tetrahedron
            if (furthest_distance < 0) {
                add_triangle(point_min, point_max, furthest_point_line);
                add_triangle(point_min, furthest_point_plane, point_max);
                add_triangle(point_min, furthest_point_line, furthest_point_plane);
                add_triangle(furthest_point_line, point_max, furthest_point_plane);
            } else {
                add_triangle(point_min, furthest_point_line, point_max);
                add_triangle(point_min, point_max, furthest_point_plane);
                add_triangle(point_min, furthest_point_plane, furthest_point_line);
                add_triangle(furthest_point_line, furthest_point_plane, point_max);
            }

            visited.push_back(point_min);
            visited.push_back(point_max);
            visited.push_back(furthest_point_line);
            visited.push_back(furthest_point_plane);

            // Create neighbors
            add_neighbours(0);
        }

        // Get other triangles
        while (!done) {
            done = true;
            glm::vec3 furthest;
            // Find Triangle for furthest point
            for (auto it = triangles.begin(); it != triangles.end(); it++) {
                glm::vec3 normal = (*it)->normal();
                furthest = mesh.find_furthest_point(normal);
                if (std::find(visited.cbegin(), visited.cend(), furthest) == visited.cend()) {
                    done = false;
                    break;
                }
            }
            if (done)
                break;
            visited.push_back(furthest);
            // Remove triangles that are facing in the wrong direction
            for (auto it = triangles.begin(); it != triangles.end();) {
                if ((*it)->outside(furthest))
                    erase_triangle(it);
                else
                    ++it;
            }
            // Create new triangles
            std::size_t current_size = triangles.size();
            for (std::size_t i = 0; i < current_size; ++i) {
                auto current_triangle = triangles[i].get();
                Triangle* new_triangle;
                if (!current_triangle->tab) {
                    new_triangle = add_triangle(furthest, current_triangle->b, current_triangle->a);
                    current_triangle->tab = new_triangle;
                    new_triangle->tbc = current_triangle;
                }
                if (!current_triangle->tbc) {
                    new_triangle = add_triangle(furthest, current_triangle->c, current_triangle->b);
                    current_triangle->tbc = new_triangle;
                    new_triangle->tbc = current_triangle;
                }
                if (!current_triangle->tca) {
                    new_triangle = add_triangle(furthest, current_triangle->a, current_triangle->c);
                    current_triangle->tca = new_triangle;
                    new_triangle->tbc = current_triangle;
                }
            }
            // Add missing neighbors
            if (!add_neighbours(current_size)) {
                Logger::warn("ConvexMeshGenerators::hull: generated an invalid triangle (This is probably a bug)");
                break;
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
            glm::vec3 normal = triangle->normal();
            Vertex v1;
            v1.position = triangle->a;
            v1.normal = normal;
            vertices.push_back(v1);
            Vertex v2;
            v2.position = triangle->b;
            v2.normal = normal;
            vertices.push_back(v2);
            Vertex v3;
            v3.position = triangle->c;
            v3.normal = normal;
            vertices.push_back(v3);
        }
        return std::make_unique<Mesh>(vertices, indices);
    }

    std::unique_ptr<Mesh> ConvexMeshGenerators::decomposition(const Mesh&) {
        // TODO: Convex decomposition
        BIRDY3D_TODO
        return nullptr;
    }

}
