#include "physics/ConvexMeshGenerators.hpp"

#include "core/Logger.hpp"
#include "render/Mesh.hpp"
#include "render/Model.hpp"

namespace Birdy3d {

    int ConvexMeshGenerators::limit = 4;

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

        Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, Triangle* tab, Triangle* tbc, Triangle* tca)
            : a(a)
            , b(b)
            , c(c)
            , tab(tab)
            , tbc(tbc)
            , tca(tca) { }

        glm::vec3 normal() const {
            glm::vec3 side1 = b - a;
            glm::vec3 side2 = a - c;
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
        std::vector<glm::vec3> visited;
        std::vector<std::unique_ptr<Triangle>> triangles;
        triangles.reserve(200); // FIXME: When reallocating the vector, the unique pointers are deleted.
        auto add_triangle = [&triangles](glm::vec3 a, glm::vec3 b, glm::vec3 c) -> Triangle* {
            auto tri = std::make_unique<Triangle>(a, b, c);
            auto ptr = tri.get();
            triangles.push_back(std::move(tri));
            return ptr;
        };
        auto erase_triangle = [&triangles](std::vector<std::unique_ptr<Triangle>>::iterator it) {
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
        std::vector<std::unique_ptr<Mesh>> meshes;
        for (const auto& mesh : model.get_meshes()) {
            triangles.clear();
            visited.clear();
            // Get first tetrahedron
            {
                // TODO: Improve creation of first tetrahedron
                glm::vec3 a = mesh->find_furthest_point(glm::vec3(1, 0, 0));
                glm::vec3 b = mesh->find_furthest_point(glm::vec3(-1, 0, 0));
                glm::vec3 c = mesh->find_furthest_point(glm::vec3(0, 1, 0));
                glm::vec3 d = mesh->find_furthest_point(glm::vec3(0, 0, 1));
                visited.push_back(a);
                visited.push_back(b);
                visited.push_back(c);
                visited.push_back(d);
                auto triangle_acb = add_triangle(a, c, b);
                auto triangle_bda = add_triangle(b, d, a);
                auto triangle_bcd = add_triangle(b, c, d);
                auto triangle_adc = add_triangle(a, d, c);
                triangle_acb->tab = triangle_adc;
                triangle_acb->tbc = triangle_bcd;
                triangle_acb->tca = triangle_bda;
                triangle_bda->tab = triangle_bcd;
                triangle_bda->tbc = triangle_adc;
                triangle_bda->tca = triangle_acb;
                triangle_bcd->tab = triangle_acb;
                triangle_bcd->tbc = triangle_adc;
                triangle_bcd->tca = triangle_bda;
                triangle_adc->tab = triangle_bda;
                triangle_adc->tbc = triangle_bcd;
                triangle_adc->tca = triangle_acb;
            }

            // Get other triangles
            bool done = false;
            while (!done) {
                done = true;
                for (std::size_t i = 0; i < triangles.size(); ++i) {
                    // For debugging
                    if (visited.size() > limit)
                        break;

                    glm::vec3 normal = triangles[i]->normal();
                    glm::vec3 furthest = mesh->find_furthest_point(normal);
                    if (std::find(visited.cbegin(), visited.cend(), furthest) != visited.cend()) {
                        /*
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
                        */
                        continue;
                    }
                    done = false;
                    visited.push_back(furthest);
                    // Remove current triangle
                    erase_triangle(triangles.begin() + i);
                    // Remove triangles that are facing in the wrong direction
                    for (auto it = triangles.begin(); it != triangles.end();) {
                        if (!(*it)->outside(furthest)) {
                            ++it;
                            continue;
                        }
                        erase_triangle(it);
                    }
                    // Create new triangles
                    auto current_end = triangles.end();
                    for (auto it = triangles.begin(); it != current_end; ++it) {
                        Triangle* tri;
                        if (!(*it)->tab) {
                            if (glm::dot(Triangle(furthest, (*it)->a, (*it)->b).normal(), (*it)->normal()) > 0)
                                tri = add_triangle(furthest, (*it)->a, (*it)->b);
                            else
                                tri = add_triangle(furthest, (*it)->b, (*it)->a);
                            (*it)->tab = tri;
                            tri->tbc = it->get();
                        }
                        if (!(*it)->tbc) {
                            if (glm::dot(Triangle(furthest, (*it)->b, (*it)->c).normal(), (*it)->normal()) > 0)
                                tri = add_triangle(furthest, (*it)->b, (*it)->c);
                            else
                                tri = add_triangle(furthest, (*it)->c, (*it)->b);
                            (*it)->tbc = tri;
                            tri->tbc = it->get();
                        }
                        if (!(*it)->tca) {
                            if (glm::dot(Triangle(furthest, (*it)->c, (*it)->a).normal(), (*it)->normal()) > 0)
                                tri = add_triangle(furthest, (*it)->c, (*it)->a);
                            else
                                tri = add_triangle(furthest, (*it)->a, (*it)->c);
                            (*it)->tca = tri;
                            tri->tbc = it->get();
                        }
                    }
                    // Add missing neighbors
                    for (auto it = current_end; it != triangles.end(); ++it) {
                        if (!(*it)->tab) {
                            for (auto& triangle : triangles) {
                                if (auto edge = triangle->get_edge((*it)->a, (*it)->b); edge != Triangle::Edge::NONE) {
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
                        }
                        if (!(*it)->tbc) {
                            for (auto& triangle : triangles) {
                                if (auto edge = triangle->get_edge((*it)->b, (*it)->c); edge != Triangle::Edge::NONE) {
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
                        }
                        if (!(*it)->tca) {
                            for (auto& triangle : triangles) {
                                if (auto edge = triangle->get_edge((*it)->c, (*it)->a); edge != Triangle::Edge::NONE) {
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
                    }
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
