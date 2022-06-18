#include "utils/PrimitiveGenerator.hpp"

#include "core/Logger.hpp"
#include "render/Mesh.hpp"
#include "render/Model.hpp"
#include <numbers>

namespace Birdy3d::utils {

    std::shared_ptr<render::Model> PrimitiveGenerator::generate_plane(unsigned int resolution) {
        std::vector<render::Vertex> vertices;
        std::vector<unsigned int> indices;
        std::size_t vertex_count = (resolution + 1) * (resolution + 1);
        std::size_t index_count = 6 * resolution * resolution;
        vertices.reserve(vertex_count);
        indices.reserve(index_count);

        float pos_step = 2.0f / resolution;
        float tex_step = 1.0f / resolution;

        for (std::size_t x = 0; x <= resolution; ++x) {
            for (std::size_t z = 0; z <= resolution; ++z) {
                // clang-format off
                vertices.emplace_back<render::Vertex>({
                    { x * pos_step - 1.0f, 0.0f, z * pos_step - 1.0f },
                    { 0.0f, 1.0f, 0.0f },
                    { x * tex_step, (resolution - z) * tex_step },
                    { -1.0f, 0.0f, 0.0f }
                });
                // clang-format on
            }
        }

        for (std::size_t v = 0; v < vertex_count - resolution - 2; ++v) {
            if ((v + 1) % (resolution + 1) == 0)
                ++v;

            indices.push_back(v);
            indices.push_back(v + 1);
            indices.push_back(v + resolution + 1);

            indices.push_back(v + 1);
            indices.push_back(v + resolution + 2);
            indices.push_back(v + resolution + 1);
        }

        auto mesh = render::Mesh { vertices, indices };
        return std::make_shared<render::Model>(std::move(mesh));
    }

    std::shared_ptr<render::Model> PrimitiveGenerator::generate_cube() {
        // clang-format off
        std::vector<render::Vertex> vertices = {
            render::Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.875f, 0.50f), glm::vec3(-1.0f, 0.0f, 0.0f) },
            render::Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.625f, 0.25f), glm::vec3(-1.0f, 0.0f, 0.0f) },
            render::Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.625f, 0.50f), glm::vec3(-1.0f, 0.0f, 0.0f) },
            render::Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.875f, 0.25f), glm::vec3(-1.0f, 0.0f, 0.0f) },

            render::Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.625f, 0.25f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            render::Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.375f, 0.00f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            render::Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.375f, 0.25f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            render::Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.625f, 0.00f), glm::vec3( 0.0f, 1.0f, 0.0f) },

            render::Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.375f, 0.50f), glm::vec3( 1.0f, 0.0f, 0.0f) },
            render::Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.125f, 0.25f), glm::vec3( 1.0f, 0.0f, 0.0f) },
            render::Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.125f, 0.50f), glm::vec3( 1.0f, 0.0f, 0.0f) },
            render::Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.375f, 0.25f), glm::vec3( 1.0f, 0.0f, 0.0f) },

            render::Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.625f, 0.50f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            render::Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.375f, 0.25f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            render::Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.375f, 0.50f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            render::Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.625f, 0.25f), glm::vec3( 0.0f, 1.0f, 0.0f) },

            render::Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.375f, 0.50f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            render::Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.625f, 0.75f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            render::Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.625f, 0.50f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            render::Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.375f, 0.75f), glm::vec3( 0.0f, 1.0f, 0.0f) },

            render::Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.625f, 0.75f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            render::Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.375f, 1.00f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            render::Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.625f, 1.00f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            render::Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.375f, 0.75f), glm::vec3( 0.0f, 1.0f, 0.0f) },
        };
        std::vector<unsigned int> indices = {
             0,  1,  2,
             0,  3,  1,

             4,  5,  6,
             4,  7,  5,

             8,  9, 10,
             8, 11,  9,

            12, 13, 14,
            12, 15, 13,

            16, 17, 18,
            16, 19, 17,

            20, 21, 22,
            20, 23, 21,
        };
        // clang-format on

        auto mesh = render::Mesh { vertices, indices };
        return std::make_shared<render::Model>(std::move(mesh));
    }

    std::shared_ptr<render::Model> PrimitiveGenerator::generate_uv_sphere(unsigned int resolution) {
        int longitude_count = resolution;
        int latitude_count = resolution;

        if (longitude_count < 2 || latitude_count < 2) {
            core::Logger::warn("Invalid uv_sphere resolution: longitude_count: {} latitude_count: {}", longitude_count, latitude_count);
            return nullptr;
        }

        std::vector<render::Vertex> vertices;
        std::vector<unsigned int> indices;

        // Vertices
        float longitude_step = (std::numbers::pi * 2) / longitude_count;
        float latitude_step = std::numbers::pi / latitude_count;

        for (int lat = 0; lat <= latitude_count; lat++) {
            for (int lon = 0; lon <= longitude_count; lon++) {
                render::Vertex v;
                v.position = glm::vec3(
                    cos(lon * longitude_step) * sin(lat * latitude_step),
                    cos(lat * latitude_step - std::numbers::pi),
                    sin(lon * longitude_step) * sin(lat * latitude_step));

                v.normal = v.position;

                v.texCoords = glm::vec2(
                    (float)lon / longitude_count,
                    (float)lat / latitude_count);

                v.tangent = glm::vec3();

                vertices.push_back(v);
            }
        }

        // Indices
        // Top Cap
        int v = longitude_count + 1;
        for (int lon = 0; lon < longitude_count; lon++, v++) {
            indices.push_back(lon);
            indices.push_back(v);
            indices.push_back(v + 1);
        }

        // Middle
        v = longitude_count + 1;
        for (int lat = 1; lat < latitude_count - 1; lat++, v++) {
            for (int lon = 0; lon < longitude_count; lon++, v++) {
                indices.push_back(v);
                indices.push_back(v + longitude_count + 1);
                indices.push_back(v + 1);

                indices.push_back(v + 1);
                indices.push_back(v + longitude_count + 1);
                indices.push_back(v + longitude_count + 2);
            }
        }

        // Bottom Cap
        for (int lon = 0; lon < longitude_count; lon++, v++) {
            indices.push_back(v);
            indices.push_back(v + longitude_count + 1);
            indices.push_back(v + 1);
        }

        auto mesh = render::Mesh { vertices, indices };
        return std::make_shared<render::Model>(std::move(mesh));
    }

    std::shared_ptr<render::Model> PrimitiveGenerator::generate_ico_sphere(unsigned int resolution [[maybe_unused]]) {
        // TODO: ico sphere primitive
        BIRDY3D_TODO
        return nullptr;
    }

}
