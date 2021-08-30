#include "scene/PrimitiveGenerator.hpp"

#include "core/Logger.hpp"
#include "render/Mesh.hpp"
#include "render/Model.hpp"

namespace Birdy3d {

    std::shared_ptr<Model> PrimitiveGenerator::generate_plane() {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // TODO: use arguments
        unsigned int res_x = 1;
        unsigned int res_z = 1;

        float x_step = 2.0f / res_x;
        float z_step = 2.0f / res_z;

        float u_step = 1.0f / res_x;
        float v_step = 1.0f / res_z;

        for (unsigned int x = 0; x <= res_x; x++) {
            for (unsigned int z = 0; z <= res_z; z++) {
                Vertex v;
                v.position = glm::vec3(x * x_step - 1, 0, z * z_step - 1);
                v.normal = glm::vec3(0, 1, 0);
                v.texCoords = glm::vec2(x * u_step, (res_z - z) * v_step);
                v.tangent = glm::vec3(1, 0, 0);

                vertices.push_back(v);
            }
        }

        for (unsigned int v = 0; v < vertices.size() - res_z - 2; v++) {
            if ((v + 1) % (res_z + 1) == 0)
                v++;

            indices.push_back(v);
            indices.push_back(v + 1);
            indices.push_back(v + res_z + 1);

            indices.push_back(v + 1);
            indices.push_back(v + res_z + 2);
            indices.push_back(v + res_z + 1);
        }

        std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(vertices, indices);
        return std::make_shared<Model>(std::move(mesh));
    }

    std::shared_ptr<Model> PrimitiveGenerator::generate_cube() {
        // clang-format off
        std::vector<Vertex> vertices = {
            Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.875f, 0.50f), glm::vec3(-1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.625f, 0.25f), glm::vec3(-1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.625f, 0.50f), glm::vec3(-1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.625f, 0.25f), glm::vec3( 0.0f, 1.0f, 0.0f) },

            Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.375f, 0.00f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.375f, 0.25f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.625f, 1.00f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.375f, 0.75f), glm::vec3( 0.0f, 1.0f, 0.0f) },

            Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.375f, 1.00f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.375f, 0.50f), glm::vec3( 1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.125f, 0.25f), glm::vec3( 1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.125f, 0.50f), glm::vec3( 1.0f, 0.0f, 0.0f) },

            Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.625f, 0.50f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.375f, 0.25f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.375f, 0.50f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.625f, 0.75f), glm::vec3( 0.0f, 1.0f, 0.0f) },

            Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.375f, 0.50f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.375f, 0.75f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.875f, 0.25f), glm::vec3(-1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.625f, 0.00f), glm::vec3( 0.0f, 1.0f, 0.0f) },

            Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.625f, 0.75f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.375f, 0.25f), glm::vec3( 1.0f, 0.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.625f, 0.25f), glm::vec3( 0.0f, 1.0f, 0.0f) },
            Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.625f, 0.50f), glm::vec3( 0.0f, 1.0f, 0.0f) },
        };
        std::vector<unsigned int> indices = {
            0,  1,  2,
            3,  4,  5,

            6,  7,  8,
            9,  10, 11,

            12, 13, 14,
            15, 16, 17,

            0,  18, 1,
            3,  19, 4,

            6,  20, 7,
            9,  21, 10,

            12, 22, 13,
            15, 23, 16
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
