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

        Mesh* mesh = new Mesh(vertices, indices);
        return std::make_shared<Model>(mesh);
    }

    std::shared_ptr<Model> PrimitiveGenerator::generate_cube() {
        // TODO
        BIRDY3D_TODO
        return nullptr;
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
