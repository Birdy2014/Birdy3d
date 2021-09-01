#include "scene/PrimitiveGenerator.hpp"

#include "core/Logger.hpp"
#include "render/Mesh.hpp"
#include "render/Model.hpp"

namespace Birdy3d {

    std::shared_ptr<Model> PrimitiveGenerator::generate_plane() {
        // clang-format off
        std::vector<Vertex> vertices = {
            Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.875f, 0.50f), glm::vec3(-1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.625f, 0.25f), glm::vec3(-1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.625f, 0.50f), glm::vec3(-1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.875f, 0.25f), glm::vec3(-1.0f, 0.0f, 0.0f) },
        };
        std::vector<unsigned int> indices = {
            0, 1, 2,
            0, 3, 1,
        };
        // clang-format on

        std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(vertices, indices);
        return std::make_shared<Model>(std::move(mesh));
    }

    std::shared_ptr<Model> PrimitiveGenerator::generate_cube() {
        // clang-format off
        std::vector<Vertex> vertices = {
            Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.875f, 0.50f), glm::vec3(-1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.625f, 0.25f), glm::vec3(-1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.625f, 0.50f), glm::vec3(-1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.875f, 0.25f), glm::vec3(-1.0f, 0.0f, 0.0f) },

            Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.625f, 0.25f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.375f, 0.00f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.375f, 0.25f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.625f, 0.00f), glm::vec3( 0.0f, 1.0f, 0.0f) },

            Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.375f, 0.50f), glm::vec3( 1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.125f, 0.25f), glm::vec3( 1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.125f, 0.50f), glm::vec3( 1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.375f, 0.25f), glm::vec3( 1.0f, 0.0f, 0.0f) },

            Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.625f, 0.50f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.375f, 0.25f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.375f, 0.50f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.625f, 0.25f), glm::vec3( 0.0f, 1.0f, 0.0f) },

            Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.375f, 0.50f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.625f, 0.75f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.625f, 0.50f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.375f, 0.75f), glm::vec3( 0.0f, 1.0f, 0.0f) },

            Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.625f, 0.75f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.375f, 1.00f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.625f, 1.00f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.375f, 0.75f), glm::vec3( 0.0f, 1.0f, 0.0f) },
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

        std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(vertices, indices);
        return std::make_shared<Model>(std::move(mesh));
    }

    std::shared_ptr<Model> PrimitiveGenerator::generate_uv_sphere(unsigned int resolution) {
        // TODO
        BIRDY3D_TODO
        return nullptr;
    }

    std::shared_ptr<Model> PrimitiveGenerator::generate_ico_sphere(unsigned int resolution) {
        // TODO
        BIRDY3D_TODO
        return nullptr;
    }

}
