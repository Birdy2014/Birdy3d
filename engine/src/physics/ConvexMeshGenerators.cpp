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
            return glm::dot(glm::vec4(n, d), glm::vec4(point, -1.0f)) >= 0;
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

        auto add_neighbours = [&triangles](std::size_t begin_offset) -> void {
            for (auto it = triangles.begin() + begin_offset; it != triangles.end(); ++it) {
                if (!(*it)->tab) {
                    for (const auto& triangle : triangles) {
                        auto edge = triangle->get_edge((*it)->a, (*it)->b);
                        if (edge == Triangle::Edge::NONE)
                            continue;
                        (*it)->tab = triangle.get();
                        if (edge == Triangle::Edge::AB)
                            triangle->tab = it->get();
                        else if (edge == Triangle::Edge::BC)
                            triangle->tbc = it->get();
                        else if (edge == Triangle::Edge::CA)
                            triangle->tca = it->get();
                        break;
                    }
                }
                if (!(*it)->tbc) {
                    for (const auto& triangle : triangles) {
                        auto edge = triangle->get_edge((*it)->b, (*it)->c);
                        if (edge == Triangle::Edge::NONE)
                            continue;
                        (*it)->tbc = triangle.get();
                        if (edge == Triangle::Edge::AB)
                            triangle->tab = it->get();
                        else if (edge == Triangle::Edge::BC)
                            triangle->tbc = it->get();
                        else if (edge == Triangle::Edge::CA)
                            triangle->tca = it->get();
                        break;
                    }
                }
                if (!(*it)->tca) {
                    for (const auto& triangle : triangles) {
                        auto edge = triangle->get_edge((*it)->c, (*it)->a);
                        if (edge == Triangle::Edge::NONE)
                            continue;
                        (*it)->tca = triangle.get();
                        if (edge == Triangle::Edge::AB)
                            triangle->tab = it->get();
                        else if (edge == Triangle::Edge::BC)
                            triangle->tbc = it->get();
                        else if (edge == Triangle::Edge::CA)
                            triangle->tca = it->get();
                        break;
                    }
                }
            }
        };

        std::vector<std::unique_ptr<Mesh>> meshes;
        for (const auto& mesh : model.get_meshes()) {
            triangles.clear();
            visited.clear();
            // Get first tetrahedron
            {
                // TODO: Improve creation of first tetrahedron.
                // TODO: Fail if tetrahedron is flat.
                glm::vec3 a = mesh->find_furthest_point(glm::vec3(1, -1, 0));
                glm::vec3 b = mesh->find_furthest_point(glm::vec3(-1, -1, 0));
                glm::vec3 c = mesh->find_furthest_point(glm::vec3(0, 1, 0));
                glm::vec3 d = mesh->find_furthest_point(glm::vec3(0, -1, 1));
                visited.push_back(a);
                visited.push_back(b);
                visited.push_back(c);
                visited.push_back(d);
                add_triangle(a, b, c);
                add_triangle(b, a, d);
                add_triangle(b, d, c);
                add_triangle(a, c, d);
                add_neighbours(0);
            }

            // Get other triangles
            bool done = false;
            while (!done) {
                done = true;
                glm::vec3 furthest;
                // Find Triangle for furthest point
                for (auto it = triangles.begin(); it != triangles.end(); it++) {
                    glm::vec3 normal = (*it)->normal();
                    furthest = mesh->find_furthest_point(normal);
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
                add_neighbours(current_size);
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
